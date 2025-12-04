use cortex_m::prelude::_embedded_hal_blocking_i2c_WriteRead;
use defmt::*;
use embassy_stm32::i2c::I2c;
use embassy_embedded_hal::shared_bus::blocking::i2c::I2cDevice;
use embassy_stm32::mode::Async;
use embassy_sync::blocking_mutex::raw::{CriticalSectionRawMutex, NoopRawMutex};
use embassy_sync::channel::Channel;
use embassy_time::{Timer, Duration};
use serde::{Deserialize, Serialize};
use {defmt_rtt as _, panic_probe as _};
use crate::adxl375::*;
use crate::iis2mdctr::*;
use crate::lsm6dsox::*;
use crate::registers::*;
use crate::lis3dh::*;
use crate::ms5611::*;

pub const SENSOR_CHANNEL_CAPACITY: usize = 8;

// millisecond wait for sensor tasks
const WAIT_TIME: u64 = 5;

#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct AccelData{
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct GyroData{
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct MagData{
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

pub static LSM_ACCEL_CHANNEL: Channel<CriticalSectionRawMutex, AccelData, 8> = Channel::new();
pub static GYRO_CHANNEL: Channel<CriticalSectionRawMutex, GyroData, 8> = Channel::new();
pub static IIS2MDCTR_CHANNEL: Channel<CriticalSectionRawMutex, MagData, 8> = Channel::new();
pub static ADXL375_1_CHANNEL: Channel<CriticalSectionRawMutex, AccelData, 8> = Channel::new();
pub static ADXL375_2_CHANNEL: Channel<CriticalSectionRawMutex, AccelData, 8> = Channel::new();
pub static LIS3DH_1_CHANNEL: Channel<CriticalSectionRawMutex, AccelData, 8> = Channel::new();
pub static LIS3DH_2_CHANNEL: Channel<CriticalSectionRawMutex, AccelData, 8> = Channel::new();
pub static MS5611_CHANNEL: Channel<CriticalSectionRawMutex, Ms5611Sample, 8> = Channel::new();

#[embassy_executor::task]
pub async fn lsm6dsox_task(
    mut i2c: I2cDevice<'static, NoopRawMutex, I2c<'static,  Async>>,
) {
    // Set up sensor (accelerometer and gyroscope configuration) before the loop
    setup_lsm6dsox(&mut i2c).await;

    let accel_scale = AccelerometerScale::Accel16g; // Set this as needed (e.g., Accel16g, Accel4g, Accel8g)
    let gyro_scale = GyroscopeScale::Dps2000; // Set this as needed (e.g., Dps125, Dps500, Dps1000, Dps2000)

    loop {
        // Check if new accelerometer data is ready
        let mut status = [0u8; 1];
        match i2c.write_read(LSM6DSOX_ADDRESS, &[LSMDSOX_STATUS_REG], &mut status) {
            Ok(()) => {
                // If the XLDA (accelerometer data available) bit is not set, continue to the next loop iteration
                if (status[0] & 0b0000_0001) == 0 {
                    continue; // No new data, skip this iteration
                }
            }
            Err(_) => {
                error!("Failed to read STATUS_REG");
                continue;
            }
        }
        
        // Read accelerometer data (6 bytes: 2 for each axis)
        let mut accel_data = [0u8; 6];
        match i2c.write_read(LSM6DSOX_ADDRESS, &[OUTX_L_A], &mut accel_data) {
            Ok(()) => {
                // Combine low and high bytes for X, Y, and Z axes using manual little-endian conversion
                let x_raw = i16::from_le_bytes([accel_data[0], accel_data[1]]) as f32;
                let y_raw = i16::from_le_bytes([accel_data[2], accel_data[3]]) as f32;
                let z_raw = i16::from_le_bytes([accel_data[4], accel_data[5]]) as f32;

                // Convert raw accelerometer data to engineering units (e.g., m/s²)
                let scale = accel_scale.to_factor();
                let x_g = x_raw * scale;
                let y_g = y_raw * scale;
                let z_g = z_raw * scale;

                let data = AccelData {
                    x: x_g,
                    y: y_g,
                    z: z_g,
                };

                //info!("Accelerometer Data (G): X = {}, Y = {}, Z = {}", x_m_s2, y_m_s2, z_m_s2);
                // Send to channel and await
                LSM_ACCEL_CHANNEL.send(data).await;
            }
            Err(_) => {
                error!("Failed to read accelerometer data");
            }
        }

        // Read gyroscope data for X, Y, Z axes (6 bytes: 2 for each axis)
        let mut gyro_data = [0u8; 6];
        match i2c.write_read(LSM6DSOX_ADDRESS, &[OUTX_L_G], &mut gyro_data) {
            Ok(()) => {
                // Combine low and high bytes for X, Y, and Z axes with Little Endian handling
                let x_raw = i16::from_le_bytes([gyro_data[0], gyro_data[1]]) as f32;
                let y_raw = i16::from_le_bytes([gyro_data[2], gyro_data[3]]) as f32;
                let z_raw = i16::from_le_bytes([gyro_data[4], gyro_data[5]]) as f32;
        
                // Convert raw gyroscope data to engineering units (e.g., degrees per second)
                let scale = gyro_scale.to_factor();
                let x_dps = x_raw * scale;
                let y_dps = y_raw * scale;
                let z_dps = z_raw * scale;

                let data = GyroData {
                    x: x_dps,
                    y: y_dps,
                    z: z_dps,
                };
        
                //info!("Gyroscope Data (°/s): X = {}, Y = {}, Z = {}", x_dps, y_dps, z_dps);
                // Send to channel and await
                GYRO_CHANNEL.send(data).await;
            }
            Err(_) => {
                error!("Failed to read gyroscope data");
            }
        }

        // Wait for a short period before the next loop iteration
        Timer::after(Duration::from_millis(WAIT_TIME)).await;
    }
}

async fn setup_lsm6dsox(i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>) {
    // 1. reset software
    let reset_command: u8 = 0x01; // SW Reset Command for LSM6DSOX (CTRL3_C)
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, CTRL3_C, 0b0000_0001, reset_command).await.is_err() {
        error!("Failed to send sensor reset command");
    }
    
    // 2. wait for software to reset
    let mut ctrl3_c_val = [0u8; 1];
    for _ in 0..5 {
        Timer::after(Duration::from_millis(10)).await;
        match i2c.write_read(LSM6DSOX_ADDRESS, &[CTRL3_C], &mut ctrl3_c_val) {
            Ok(()) => {
                if ctrl3_c_val[0] & 0x01 == 0 {
                    break;
                }
            }
            Err(_) => {
                error!("Failed to read CTRL3_C during reset check");
            }
        }
    }

    // 3. Disable I3C and enable block data update using update_reg_address
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, CTRL9_XL, 0b0000_0011, 0x03).await.is_err() {
        error!("Failed to configure I3C interface");
    }
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, I3C_BUS_AVB, 0b0001_1000, 0b0001_1000).await.is_err() {
        error!("Failed to configure I3C Bus AVB");
    }
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, CTRL3_C, 0b0100_0000, 0b0100_0000).await.is_err() {
        error!("Failed to enable Block Data Update");
    }

    // 4. Set accel and gyro scale using update_reg_address
    let accel_odr = DataRate::Freq208Hz as u8;  // 0xA0
    let accel_scale = AccelerometerScale::Accel16g as u8;  // 0x04
    let accel_config = accel_odr | accel_scale;
    info!("Accel config: {:#X}", accel_config);
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, CTRL1_XL, 0b1111_1100, accel_config).await.is_err() {
        error!("Failed to configure CTRL1_XL");
    }

    let gyro_odr = DataRate::Freq208Hz as u8;
    let gyro_scale = GyroscopeScale::Dps2000 as u8;
    let gyro_config = gyro_odr | gyro_scale;
    if update_reg_address(i2c, LSM6DSOX_ADDRESS, CTRL2_G, 0b1111_1100, gyro_config).await.is_err() {
        error!("Failed to configure CTRL2_G");
    }
}

#[embassy_executor::task(pool_size = 2)]
pub async fn adxl375_task(
    mut i2c: I2cDevice<'static, NoopRawMutex, I2c<'static,  Async>>,
    address: u8
) {
    // setup the adxl375 we want
    setup_adxl375(&mut i2c, address).await;

    loop {
        // check if new accelerometer data is ready
        let mut status = [0u8; 1];
        match i2c.write_read(address, &[INT_SOURCE], &mut status) {
            Ok(()) => {
                // if dataready bit is not set, continue to next iteration
                if (status[0] & 0b1000_0000) == 0 {
                    continue;
                }
            }
            Err(_) => {
                warn!("Failed to read {} INT_SOURCE", address);
                continue;
            }
        }

        // read accelerometer data (6 bytes, 2 for each axis)
        let mut accel_data= [0u8; 6];
        match i2c.write_read(address, &[DATAX0], &mut accel_data) {
            Ok(()) => {
                // Combine low and high bytes for X, Y, and Z
                let x_raw = i16::from_le_bytes([accel_data[0], accel_data[1]]) as f32;
                let y_raw = i16::from_le_bytes([accel_data[2], accel_data[3]]) as f32;
                let z_raw = i16::from_le_bytes([accel_data[4], accel_data[5]]) as f32;
                
                let scale = ADXL375_SCALE_FACTOR;
                let x_g = x_raw * scale;
                let y_g = y_raw * scale;
                let z_g = z_raw * scale;

                let data =  AccelData {
                    x: x_g,
                    y: y_g,
                    z: z_g,
                };

                //info!("ADXL375 Data (G): X = {}, Y = {}, Z = {}", x_g, y_g, z_g);

                // Seperate data for each sensor
                if address == ADXL375_LOW_ADDRESS {
                    ADXL375_1_CHANNEL.send(data).await;
                }
                else if address == ADXL375_HIGH_ADDRESS {
                    ADXL375_2_CHANNEL.send(data).await;
                }
            }
            Err(_) => {
                warn!("Failed to read ADXL375 {} data", address);
            }
        }

        // Wait for a short period before the next loop iteration
        Timer::after(Duration::from_millis(WAIT_TIME)).await;
    }
}

pub async fn setup_adxl375(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8
) {
    // 1. Set the ODR and bandwidth
    if update_reg_address(i2c, address, BW_RATE, 0b0000_1111, ADXL375_200HZ).await.is_err() {
        error!("Failed to configure ADXL375 {} ODR", address);
    }

    // 2. Set the FIFO mode (bypass, stream, FIFO, trigger)
    if update_reg_address(i2c, address, FIFO_CTL, 0b1100_0000, ADXL375_FIFO_BYPASS).await.is_err() {
        error!("Failed to configure ADXL375 {} FIFO", address);
    }

    // 3. Set the data format to right justified
    if update_reg_address(i2c, address, DATA_FORMAT, 0b0000_0100, ADXL375_JUSTIFY_RIGHT).await.is_err() {
        error!("Failed to configure ADXL375 {} data justification", address);
    }

    // 4. finally, set the power mode to measure and turn off sleep
    if update_reg_address(i2c, address, POWER_CTL, 0b0000_1100, ADXL375_MEASURE_MODE).await.is_err() {
        error!("Failed to put ADXL375 {} into measure mode", address);
    }
    
    // 5. apply pre-calibrated offsets
    if address == ADXL375_LOW_ADDRESS {
        // Apply offsets for the first sensor
        apply_adxl375_offsets(i2c, address, ADXL375_1_OFFSET_X, ADXL375_1_OFFSET_Y, ADXL375_1_OFFSET_Z).await;
    } else if address == ADXL375_HIGH_ADDRESS {
        // Apply offsets for the second sensor
        apply_adxl375_offsets(i2c, address, ADXL375_2_OFFSET_X, ADXL375_2_OFFSET_Y, ADXL375_2_OFFSET_Z).await;
    }
    
    // Only need to do this once per sensor, then use above to apply them every runtime.
    //calibrate_offsets(i2c, address, 10, 100).await;
}

async fn apply_adxl375_offsets(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8,
    x_offset: i16,
    y_offset: i16,
    z_offset: i16,
) {
    // Write the offsets to the sensor's offset registers
    if  update_reg_address(i2c, address, OFSX, 0xFF, -x_offset as u8).await.is_err() ||
        update_reg_address(i2c, address, OFSY, 0xFF, -y_offset as u8).await.is_err() ||
        update_reg_address(i2c, address, OFSZ, 0xFF, -z_offset as u8).await.is_err() {
        error!("Failed to apply offsets to ADXL375 {} offset registers", address);
    }
}

pub async fn calibrate_offsets(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8,
    num_samples: usize, // Number of samples to average
    sample_rate_hz: u32, // Sample rate in Hz (should be >= 100 Hz)
) {
    let mut x_sum = 0i16;
    let mut y_sum = 0i16;
    let mut z_sum = 0i16;

    // Step 1: Sample the sensor multiple times to get an average
    for _ in 0..num_samples {
        let mut accel_data = [0u8; 6];
        if i2c.write_read(address, &[DATAX0], &mut accel_data).is_ok() {
            let x_raw = i16::from_le_bytes([accel_data[0], accel_data[1]]);
            let y_raw = i16::from_le_bytes([accel_data[2], accel_data[3]]);
            let z_raw = i16::from_le_bytes([accel_data[4], accel_data[5]]);

            // Accumulate raw values for averaging
            x_sum += x_raw;
            y_sum += y_raw;
            z_sum += z_raw;
        } else {
            error!("Failed to read ADXL375 {} data", address);
            return;
        }

        // Optional: Add a delay to match the sample rate
        Timer::after_millis(1000/sample_rate_hz as u64).await;
    }

    // Step 2: Calculate the average for each axis
    let x_avg = x_sum as f32 / num_samples as f32;
    let y_avg = y_sum as f32 / num_samples as f32;
    let z_avg = z_sum as f32 / num_samples as f32;

    // Step 3: Calculate the offset values
    // For X and Y, subtract the 0g value (which should be close to zero)
    let x_offset = (x_avg * ADXL375_OFFSET_SCALE) as i16;
    let y_offset = (y_avg * ADXL375_OFFSET_SCALE) as i16;

    // For Z, subtract 1g field value (assumes ideal sensitivity of SZ LSB/g)
    let z_offset = (z_avg * ADXL375_OFFSET_SCALE - 1.0) as i16; // Adjust for 1g field

    // Step 4: Write the calculated offsets to the offset registers
    if update_reg_address(i2c, address, OFSX, 0xFF, -x_offset as u8).await.is_err() ||
        update_reg_address(i2c, address, OFSY, 0xFF, -y_offset as u8).await.is_err() ||
        update_reg_address(i2c, address, OFSZ, 0xFF, -z_offset as u8).await.is_err() {
        error!("Failed to write ADXL375 {} offset registers", address);
        return;
    }

    // Step 5: Log the results
    info!(
        "ADXL375 {} Calibration: X Offset = {}, Y Offset = {}, Z Offset = {}",
        address, x_offset, y_offset, z_offset
    );
}

#[embassy_executor::task(pool_size = 2)]
pub async fn lis3dh_task(
    mut i2c: I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8
) {
   // run setup function first
    setup_lis3dh(&mut i2c, address).await;

    loop {
        // Check if new accelerometer data is ready to read
        let mut status = [0u8; 1];
        match i2c.write_read(address, &[LIS3DH_STATUS_REG], &mut status) {
            Ok(()) => {
                //info!("Lis3dh status register: 0x{:x}", status[0]);
                // If the XLDA (accelerometer data available) bit is not set, continue to the next loop iteration
                if (status[0] & LIS3DH_DATA_RDY) == 0 {
                    continue; // No new data, skip this iteration
                }
            }
            Err(_) => {
                error!("Failed to read LIS3 STATUS_REG");
                continue;
            }
        }

        // Read accelerometer data
        let mut accel_data = [0u8; 6];
        // Read the first register for data while setting MSB (7) to a 1 for auto-increment.
        match i2c.write_read(address, &[START_REG], &mut accel_data) {
            Ok(()) => {
                // Convert raw bytes to i16 values
                let x_raw = i16::from_le_bytes([accel_data[0], accel_data[1]]) >> LIS3DH_SHIFT_HR;
                let y_raw = i16::from_le_bytes([accel_data[2], accel_data[3]]) >> LIS3DH_SHIFT_HR;
                let z_raw = i16::from_le_bytes([accel_data[4], accel_data[5]]) >> LIS3DH_SHIFT_HR;

                let x_g = x_raw as f32 * LIS3DH_SCALE_16G;
                let y_g = y_raw as f32 * LIS3DH_SCALE_16G;
                let z_g = z_raw as f32 * LIS3DH_SCALE_16G;

                let data = AccelData {
                    x: x_g,
                    y: y_g,
                    z: z_g,
                };

                // Seperate data for each sensor
                if address == LIS3DH_LOW_ADDRESS {
                    LIS3DH_1_CHANNEL.send(data).await;
                }
                else if address == LIS3DH_HIGH_ADDRESS {
                    LIS3DH_2_CHANNEL.send(data).await;
                }
            }
            Err(_) => {
                error!("Failed to read LIS3DH accelerometer data");
            }
        }

        // Wait for a short period before the next loop iteration
        Timer::after(Duration::from_millis(WAIT_TIME)).await;
    }
}

async fn setup_lis3dh(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8
) {
    // 1. set HR mode
    if update_reg_address(i2c, address, CTRL_REG4, 0b0000_1000, LIS3DH_HIGH_RES).await.is_err() {
        error!("Failed to set lis3dh HR mode");
    }

    // wait for the HR mode to be set
    Timer::after_millis(50).await;

    // 2. set ODR
    if update_reg_address(i2c, address, CTRL_REG1, 0b1111_0000, LIS3DH_200Hz).await.is_err() {
        error!("Failed to set lis3dh ODR");
    }
    debug_register(i2c, address, CTRL_REG1, "CTRL_REG1").await;

    // 3. set scale and enable bdu
    let accel_scale = LIS3DH_16G;
    let bdu_enable = LIS3DH_BDU;
    let config = accel_scale | bdu_enable;
    if update_reg_address(i2c, address, CTRL_REG4, 0b1011_0000, config).await.is_err() {
        error!("Failed to set lis3dh scale and bdu");
    }
    debug_register(i2c, address, CTRL_REG4, "CTRL_REG4").await;

    // 4. turn the internal pullup off for SA0
    if update_reg_address(i2c, address, CTRL_REG0, 0b1000_0000, LIS3DH_SA0_PU_DISABLE).await.is_err() {
        error!("Failed to disable lis3dh sa0 pullup");
    }
}

#[embassy_executor::task]
pub async fn iis2mdctr_task(mut i2c: I2cDevice<'static, NoopRawMutex, I2c<'static,  Async>>) {
    // set up the sensor
    setup_iis2mdctr(&mut i2c).await;

    loop {
        // check if data is ready for xyz
        let mut status = [0u8; 1];
        match i2c.write_read(IIS2MDCTR_ADDRESS, &[STATUS_REG], &mut status) {
            Ok(()) => {
                if (status[0] & IIS2MDCTR_DATA_RDY) == 0 {
                    continue;
                }
            }
            Err(_) => {
                error!("Failed to read MAG STATUS_REG");
            }
        }

        // Read accelerometer data
        let mut mag_data = [0u8; 6];
        // Read the first register for data while setting MSB (7) to a 1 for auto-increment.
        match i2c.write_read(IIS2MDCTR_ADDRESS, &[BEGIN_READ_REG], &mut mag_data) {
            Ok(()) => {
                // Convert raw bytes to i16 values
                let x_raw = i16::from_le_bytes([mag_data[0], mag_data[1]]);
                let y_raw = i16::from_le_bytes([mag_data[2], mag_data[3]]);
                let z_raw = i16::from_le_bytes([mag_data[4], mag_data[5]]);

                let x_g = x_raw as f32 * IIS2MDCTR_GAUSS_SCALE;
                let y_g = y_raw as f32 * IIS2MDCTR_GAUSS_SCALE;
                let z_g = z_raw as f32 * IIS2MDCTR_GAUSS_SCALE;

                let data = MagData {
                    x: x_g,
                    y: y_g,
                    z: z_g,
                };

                // Send data to the channel
                IIS2MDCTR_CHANNEL.send(data).await;
            }
            Err(_) => {
                error!("Failed to read MAG data");
            }
        }
        
        Timer::after_millis(WAIT_TIME).await;
    }
}

async fn setup_iis2mdctr(i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>) {
    // 1. reset software
    let reset_command: u8 = 0b0100_0000; // SW Reset Command for LSM6DSOX (CTRL3_C)
    if update_reg_address(i2c, IIS2MDCTR_ADDRESS, CFG_REG_A, 0b0100_0000, reset_command).await.is_err() {
        error!("Failed to send MAG reset command");
    }
    
    Timer::after_millis(50).await;
    
    // 2. enable temperature compensation, set ODR, enable continuous mode
    let config = IIS2MDCTR_CONTINUOUS_MODE | IIS2MDCTR_TEMP_EN | IIS2MDCTR_ODR_100Hz;
    if update_reg_address(i2c, IIS2MDCTR_ADDRESS, CFG_REG_A, 0b1000_1111, config).await.is_err() {
        error!("Failed to set ODR, Continuous mode, and temperature compensation");
    }
    
    // 3. Enable block data update (BDU)
    if update_reg_address(i2c, IIS2MDCTR_ADDRESS, CFG_REG_C, 0b0001_0000, IIS2MDCTR_BDU_EN).await.is_err() {
        error!("Failed to enable BDU");
    }
}

#[embassy_executor::task]
pub async fn ms5611_task(mut i2c: I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>) {
    // reset device then read PROM calibration data
    let prom_data = match setup_ms5611(&mut i2c).await {
        Ok(data) => data,
        Err(_) => {
            error!("MS5611 setup failed");
            return;
        }
    };

    loop {
        // read temperature and pressure data
        // osr of 2048 => ~110 Hz, 1024 => ~218 Hz
        match read_sample(&mut i2c, &prom_data, Osr::Opt512).await {
            Ok(sample) => {
                // info!(
                //     "MS5611: Temp = {} °C, Pressure = {} mbar",
                //     sample.temp_c,
                //     sample.pressure_mbar,
                // );
                
                MS5611_CHANNEL.send(sample).await;
            }
            Err(_) => {
                error!("MS5611 read_sample failed");
            }
        }
        
        //Timer::after_millis(WAIT_TIME).await;
    }
}

async fn setup_ms5611(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
) -> Result<PromData, ()> {
    reset(i2c).await?;  // Reset the sensor
    let prom = read_prom(i2c).await?;  // get the PROM data for use later

    // info!("PromData: c1-{}, c2-{}, c3-{}, c4-{}, c5-{}, c6-{}, ", 
    //     prom.pressure_sensitivity, prom.pressure_offset,
    //     prom.temp_coef_pressure_sensitivity, prom.temp_coef_pressure_offset,
    //     prom.temp_ref, prom.temp_coef_temp
    // );    

    Ok(prom)
}
