#![no_std]
#![no_main]

mod sensors;
mod lsm6dsox;
mod registers;
mod usb_serial;
mod adxl375;
mod lis3dh;
mod iis2mdctr;
mod ms5611;
mod gps;
mod can_tx;
mod w25q128jvs;

use core::any::Any;
use core::cell::RefCell;
use defmt::*;
use embassy_embedded_hal::shared_bus::blocking::i2c::I2cDevice;
use embassy_executor::Spawner;
use embassy_stm32::{bind_interrupts, i2c, peripherals, Config, can, usart, ospi, spi};
use embassy_stm32::can::config::{FdCanConfig, TxBufferMode};
use embassy_stm32::gpio::{Flex, Level, Output, Speed};
use embassy_stm32::i2c::I2c;
use embassy_stm32::mode::Async;
use embassy_stm32::ospi::{ChipSelectHighTime, MemorySize, MemoryType, Ospi};
use embassy_stm32::peripherals::{FDCAN1, OCTOSPI1};
use embassy_stm32::rcc::mux::I2csel;
use embassy_stm32::time::Hertz;
use embassy_stm32::usart::{DataBits, Parity, StopBits, Uart};
use embassy_sync::blocking_mutex::NoopMutex;
use embassy_sync::blocking_mutex::raw::{CriticalSectionRawMutex, NoopRawMutex, ThreadModeRawMutex};
use embassy_sync::pubsub::{PubSubBehavior, PubSubChannel};
use embassy_time::{Timer, Instant};
use static_cell::StaticCell;
use crate::adxl375::{ADXL375_LOW_ADDRESS, ADXL375_HIGH_ADDRESS};
use crate::gps::*;
use crate::lis3dh::{LIS3DH_HIGH_ADDRESS, LIS3DH_LOW_ADDRESS};
use crate::ms5611::Ms5611Sample;
use crate::sensors::*;
use crate::usb_serial::*;
use serde::{Serialize, Deserialize};
use crate::can_tx::can_tx_task;
use embassy_stm32::rcc::*;
use embassy_sync::mutex::Mutex;
use crate::w25q128jvs::{store_sensor_data, FlashMemory};

static I2C_BUS: StaticCell<NoopMutex<RefCell<I2c<'static, Async>>>> = StaticCell::new();
static FLASH: Mutex<CriticalSectionRawMutex, Option<FlashMemory<OCTOSPI1>>> = Mutex::new(None);

// Combine all sensor data into one struct for packetization
#[derive(Clone, Default, Serialize, Deserialize)]
pub struct SensorPacket {
    pub time: u64,
    pub lsm_accel: AccelData,
    pub gyro: GyroData,
    pub mag: MagData,
    pub baro: Ms5611Sample,
    pub adxl_1: AccelData,
    pub adxl_2: AccelData,
    pub lis_1: AccelData,
    pub lis_2: AccelData,
    pub gps: GpsRmc,
}

pub const SENSOR_SUBSCRIBERS: usize = 3; // how many subscribers to this channel
pub const SENSOR_PUBLISHERS: usize = 1;  // how many publishers (senders)
pub static SENSOR_PUBSUB: PubSubChannel<CriticalSectionRawMutex, SensorPacket, 8, SENSOR_SUBSCRIBERS, SENSOR_PUBLISHERS> = PubSubChannel::new();

bind_interrupts!(struct I2cIrqs {
    I2C1_EV => i2c::EventInterruptHandler<peripherals::I2C1>;
    I2C1_ER => i2c::ErrorInterruptHandler<peripherals::I2C1>;
});

bind_interrupts!(struct CanIrqs {
    FDCAN1_IT0 => can::IT0InterruptHandler<FDCAN1>;
    FDCAN1_IT1 => can::IT1InterruptHandler<FDCAN1>;
});

bind_interrupts!(struct UsartIrqs {
    USART1 => usart::InterruptHandler<peripherals::USART1>;
});

// Main function and entry point of the program after configuration
#[embassy_executor::main]
async fn main(spawner: Spawner) {
    let mut config = Config::default();
    config.rcc.hsi = false;  // disable hsi clock
    config.rcc.hse = Some(Hse{
        freq: Hertz(16_000_000),
        mode: HseMode::Oscillator,
    });  // set the hse clock speed and type to the one on board
    config.rcc.pll1 = Some(Pll {
        source: PllSource::HSE, // 16 MHz external clock
        prediv: PllPreDiv::DIV1,
        mul: PllMul::MUL10,       // 160MHz (multiply by 10)
        divp: Some(PllDiv::DIV1),
        divq: Some(PllDiv::DIV1),
        divr: Some(PllDiv::DIV1),
    });
    config.rcc.sys = Sysclk::PLL1_R;
    config.rcc.voltage_range = VoltageScale::RANGE1;
    config.rcc.hsi48 = Some(Hsi48Config { sync_from_usb: true }); // needed for USB
    config.rcc.mux.iclksel = mux::Iclksel::HSI48; // USB uses ICLK (48MHz)
    config.rcc.mux.i2c1sel = I2csel::SYS;
    config.rcc.mux.fdcan1sel = mux::Fdcansel::HSE;  // 16MHz

    // Setup peripherals on the default clock
    let p = embassy_stm32::init(config);

    // flash setup for winbond W25Q128JVS
    let mut flash_config = ospi::Config::default();
    flash_config.chip_select_high_time = ChipSelectHighTime::_8Cycle;  // standard SPI needs 8 cycles
    flash_config.clock_prescaler = 16;  // since our clock is 160MHz, we need to slow it down a bit (10 Mhz)
    flash_config.device_size = MemorySize::_128MiB;  // set memory size so the device knows how much to shift
    let _wp = Output::new(p.PA7, Level::High, Speed::Low);  // disable io2 and io3 by pulling high
    let _hold = Output::new(p.PA6, Level::High, Speed::Low);
    let spi = Ospi::new_singlespi(p.OCTOSPI1, p.PA3, p.PB1, p.PB0, p.PA0, p.GPDMA1_CH3, flash_config);
    let flash = FlashMemory::new(spi);
    // initialize the global
    {
        let mut guard = FLASH.lock().await;
        *guard = Some(flash);
    }

    // stanby pin of the CAN controller and force low for normal operation mode
    let mut can_stby = Output::new(p.PD3, Level::Low, Speed::Medium);
    can_stby.set_low();
    Timer::after_millis(10).await;
    // can configurator with pins!
    let mut can = can::CanConfigurator::new(p.FDCAN1, p.PD0, p.PD1, CanIrqs);
    can.set_bitrate(250_000);
    let mut can = can.into_normal_mode();

    // Set up user led as output
    let mut led = Output::new(p.PE2, Level::Low, Speed::Medium);
    
    // Use I2C1 for sensors with default clock
    let i2c = I2c::new(p.I2C1, p.PB6, p.PB3, I2cIrqs, p.GPDMA1_CH4, p.GPDMA1_CH5, Hertz(100_000), Default::default());

    // Set stby gps pin to a flex for switching between low and floating
    // Pulse the STANDBY pin to wake up the GPS
    let gps_stby = Flex::new(p.PC9);
    // Pull reset pin high
    let mut gps_reset = Output::new(p.PA8, Level::High, Speed::Low);
    
    // Default gps settings
    let mut gps_config = usart::Config::default();
    gps_config.baudrate = 9600;
    gps_config.data_bits = DataBits::DataBits8;
    gps_config.parity = Parity::ParityNone;
    gps_config.stop_bits = StopBits::STOP1;
    // GPS uses USART1
    let mut gps = Uart::new(
        p.USART1,
        p.PA10,       // TX to send commands to GPS
        p.PA9,        // RX from GPS
        UsartIrqs,
        p.GPDMA1_CH0,   // TX channel
        p.GPDMA1_CH1,   // RX channel
        gps_config,
    ).unwrap();
    let (mut tx, mut rx) = gps.split();
    setup_gps(&mut tx, &mut rx, gps_stby).await;
    // TEST GPS READING
    spawner.spawn(gps_reader(rx)).unwrap();
    
    let i2c_bus = NoopMutex::new(RefCell::new(i2c));
    let i2c_bus = I2C_BUS.init(i2c_bus);

    let i2c_lsm6dsox = I2cDevice::new(i2c_bus);
    let i2c_iis2mdctr = I2cDevice::new(i2c_bus);
    let i2c_ms5611 = I2cDevice::new(i2c_bus);
    let i2c_adxl375_1 = I2cDevice::new(i2c_bus);
    let i2c_adxl375_2 = I2cDevice::new(i2c_bus);
    let i2c_lis3dh_1 = I2cDevice::new(i2c_bus);
    let i2c_lis3dh_2 = I2cDevice::new(i2c_bus);

    spawner.spawn(lsm6dsox_task(i2c_lsm6dsox)).unwrap();
    spawner.spawn(iis2mdctr_task(i2c_iis2mdctr)).unwrap();
    spawner.spawn(ms5611_task(i2c_ms5611)).unwrap();
    
    // // Comment below if unplugged
    spawner.spawn(adxl375_task(i2c_adxl375_1, ADXL375_LOW_ADDRESS)).unwrap();
    spawner.spawn(adxl375_task(i2c_adxl375_2, ADXL375_HIGH_ADDRESS)).unwrap();
    spawner.spawn(lis3dh_task(i2c_lis3dh_1, LIS3DH_LOW_ADDRESS)).unwrap();
    spawner.spawn(lis3dh_task(i2c_lis3dh_2, LIS3DH_HIGH_ADDRESS)).unwrap();
    
    Timer::after_millis(1000).await;
    
    spawner.spawn(aggregator_task()).unwrap();  // sensor data aggregation
    // spawn the two tasks that both need flash
    spawner.spawn(store_sensor_data()).unwrap();
    spawner.spawn(usb_task(p.USB, p.PA12, p.PA11, &SENSOR_PUBSUB)).unwrap();
    // Take ownership of TX side
    let (can_tx, _can_rx, _props) = can.split();
    spawner.spawn(can_tx_task(can_tx)).unwrap();

    loop {
        led.set_high();
        Timer::after_millis(500).await;
        led.set_low();
        Timer::after_millis(500).await;
    }
}

#[embassy_executor::task]
async fn aggregator_task() {
    fn try_receive_to<T: core::fmt::Debug>(
        channel: &embassy_sync::channel::Channel<CriticalSectionRawMutex, T, 8>,
        dest: &mut T,
        label: &str,
    ) {
        match channel.try_receive() {
            Ok(data) => *dest = data,
            Err(e) => info!("Error receiving {} data: {:?}", label, e),
        }
    }
    
    loop {
        let mut data_packet = SensorPacket {
            time: Instant::now().as_millis(),
            lsm_accel: AccelData::default(),
            gyro: GyroData::default(),
            mag: MagData::default(),
            baro: Ms5611Sample::default(),
            adxl_1: AccelData::default(),
            adxl_2: AccelData::default(),
            lis_1: AccelData::default(),
            lis_2: AccelData::default(),
            gps: GpsRmc::default(),
        };
        
        try_receive_to(&LSM_ACCEL_CHANNEL, &mut data_packet.lsm_accel, "LSM accel");
        try_receive_to(&GYRO_CHANNEL, &mut data_packet.gyro, "gyro");
        try_receive_to(&IIS2MDCTR_CHANNEL, &mut data_packet.mag, "IIS2");
        try_receive_to(&MS5611_CHANNEL, &mut data_packet.baro, "MS5611");

        // // Comment below if unplugged
        try_receive_to(&ADXL375_1_CHANNEL, &mut data_packet.adxl_1, "ADXL1");
        try_receive_to(&ADXL375_2_CHANNEL, &mut data_packet.adxl_2, "ADXL2");
        try_receive_to(&LIS3DH_1_CHANNEL, &mut data_packet.lis_1, "LIS1");
        try_receive_to(&LIS3DH_2_CHANNEL, &mut data_packet.lis_2, "LIS2");
        
        //info!("Mag Data (gauss): X = {}, Y = {}, Z = {}", data_packet.mag.x, data_packet.mag.y, data_packet.mag.z);

        // Retrieve the most recent GPS data from the Mutex.
        {
            let gps_lock = LATEST_GPS.lock().await;
            // Clone the current GPS value. If no update has ever arrived, this remains None.
            data_packet.gps = gps_lock.borrow().clone();
        }
        
        // Broadcast this packet to telemetry consumers (data storage, radio, CAN)
        // This will publish without waiting for an empty slot. change to a publisher to correctly wait for space with "publish()"
        SENSOR_PUBSUB.publish_immediate(data_packet);
        
        // adjust timer based on how fast each subscriber needs the data
        Timer::after_millis(10).await;
    }
}
