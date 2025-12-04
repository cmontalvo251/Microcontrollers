use defmt::{panic, *};
use defmt_rtt as _; // global logger
use embassy_futures::join::join;
use embassy_stm32::usb::{DmPin, DpPin, Driver, Instance};
use embassy_stm32::{bind_interrupts, peripherals, usb, Peri};
use embassy_stm32::peripherals::{OCTOSPI1, USB};
use embassy_usb::class::cdc_acm::{CdcAcmClass, State};
use embassy_usb::driver::EndpointError;
use embassy_usb::Builder;
use panic_probe as _;
use core::fmt::{Write};
use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::pubsub::{PubSubChannel, WaitResult};
use embassy_time::Timer;
use heapless::String;
use crate::{SensorPacket, FLASH};
use crate::gps::convert_to_decimal;
use crate::w25q128jvs::{FlashMemory, Header, HEADER_MAGIC, HEADER_SLOTS, PAGE_SIZE, SECTOR_SIZE};

bind_interrupts!(struct Irqs {
    USB => usb::InterruptHandler<peripherals::USB>;
});

async fn usb_print<'a, T: Instance>(
    class: &mut CdcAcmClass<'a, Driver<'a, T>>,
    args: core::fmt::Arguments<'_>,
) {
    let mut buffer: String<256> = String::new(); // adjust size as needed
    let _ = buffer.write_fmt(args);
    
    // Send in chunks of 64 or less
    let mut i = 0;
    while i < buffer.len() {
        let end = (i + 64).min(buffer.len());
        let chunk = &buffer.as_bytes()[i..end];
        if let Err(e) = class.write_packet(chunk).await {
            warn!("USB write error: {:?}", e);
            break;
        }
        i = end;
    }

    // If last chunk was exactly 64 bytes, send a ZLP
    if buffer.len() % 64 == 0 {
        let _ = class.write_packet(&[]).await;
    }
}

// Handy macro
#[macro_export]
macro_rules! usb_write {
    ($usb:expr, $($arg:tt)*) => {
        usb_print($usb, format_args!($($arg)*)).await
    };
}

#[embassy_executor::task]
pub async fn usb_task(
    usb: Peri<'static, USB>,
    dp: Peri<'static, peripherals::PA12>,
    dm: Peri<'static, peripherals::PA11>,
    pub_sub_channel: &'static PubSubChannel<CriticalSectionRawMutex, SensorPacket, 8, 3, 1>,
) {
    // Do not need the vbus protection config since it doesn't exist for this chip.
    let driver = Driver::new(usb, Irqs, dp, dm);

    // Create embassy-usb Config.
    let mut config = embassy_usb::Config::new(0xc0de, 0xcafe);
    config.manufacturer = Some("Embassy");
    config.product = Some("Main Sensor Board");
    config.serial_number = Some("12345678");

    // Buffers for building the descriptors.
    let mut config_descriptor = [0; 256];
    let mut bos_descriptor = [0; 256];
    let mut control_buf = [0; 64];

    let mut state = State::new();

    let mut builder = Builder::new(
        driver,
        config,
        &mut config_descriptor,
        &mut bos_descriptor,
        &mut [], // no msos descriptors
        &mut control_buf,
    );

    // Create classes on the builder.
    let mut class = CdcAcmClass::new(&mut builder, &mut state, 64);

    // Build the builder.
    let mut usb = builder.build();

    // Run the USB device.
    let usb_fut = usb.run();

    // Create the subscriber for the usb_serial.
    let mut usb_subscriber = pub_sub_channel.subscriber().unwrap();

    // The echo task, which waits for a connection and then forwards sensor data.
    let echo_fut = async {
        loop {
            class.wait_connection().await;
            info!("USB connected, awaiting command…");

            // Prompt the host
            let _ = class.write_packet(b"Press 'D' to dump log\r\n").await;

            // Buffer for incoming commands
            let mut b = [0u8; 1];

            // 1) Wait for the “DUMP” command
            loop {
                let n = class.read_packet(&mut b).await.unwrap_or(0);
                if n == 1 {
                    if b[0] == b'D' || b[0] == b'd' {
                        break;
                    } else {
                        // echo back the invalid key and prompt again
                        let _ = class.write_packet(b"Invalid, press 'D'\r\n").await;
                    }
                }
                // if n==0, just loop
            }

            // 2) Dump the stored flash after connecting
            {
                let mut guard = FLASH.lock().await;
                let flash = guard.as_mut().unwrap();

                // Read header from Sector 0
                let mut buf0 = [0u8; PAGE_SIZE];
                flash.read_data(0, &mut buf0).await.unwrap();
                let hdr0: Header = postcard::from_bytes(&buf0[..size_of::<Header>()]).unwrap_or_default();

                // Read header from Sector 1
                let mut buf1 = [0u8; PAGE_SIZE];
                flash.read_data(SECTOR_SIZE as u32, &mut buf1).await.unwrap();
                let hdr1: Header = postcard::from_bytes(&buf1[..size_of::<Header>()]).unwrap_or_default();

                // Pick the newest valid header
                let best = if hdr0.marker == HEADER_MAGIC && hdr0.version >= hdr1.version {
                    hdr0
                } else if hdr1.marker == HEADER_MAGIC {
                    hdr1
                } else {
                    Header::default()
                };

                if best.marker != HEADER_MAGIC || best.pages == 0 {
                    let _ = class.write_packet(b"No log to dump\r\n").await;
                } else {
                    info!("Dumping {} pages…", best.pages);
                    // send CSV header
                    let _ = class.write_packet(
                        b"time,ax,ay,az, gx,gy,gz, mx,my,mz, pres,temp, ...\r\n"
                    ).await;

                    for i in 0..best.pages as usize {
                        let addr = SECTOR_SIZE as u32 + (i as u32) * PAGE_SIZE as u32;
                        let mut page_buf = [0u8; PAGE_SIZE];
                        if flash.read_data(addr, &mut page_buf).await.is_err() {
                            info!("Read error at page {}, skipping", i);
                            continue;
                        }
                        // safely deserialize or skip
                        match postcard::from_bytes::<SensorPacket>(&page_buf) {
                            Ok(packet) => {
                                usb_write!(
                                &mut class,
                                "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}\r\n",
                                packet.time,
                                packet.lsm_accel.x, packet.lsm_accel.y, packet.lsm_accel.z,
                                packet.gyro.x,    packet.gyro.y,    packet.gyro.z,
                                packet.mag.x,     packet.mag.y,     packet.mag.z,
                                packet.baro.pressure_mbar, packet.baro.temp_c,
                                packet.adxl_1.x,  packet.adxl_1.y,  packet.adxl_1.z,
                                packet.adxl_2.x,  packet.adxl_2.y,  packet.adxl_2.z,
                                packet.lis_1.x,   packet.lis_1.y,   packet.lis_1.z,
                                packet.lis_2.x,   packet.lis_2.y,   packet.lis_2.z,
                                packet.gps.status as char,
                                packet.gps.utc_time, packet.gps.date,
                                packet.gps.latitude, packet.gps.longitude,
                                packet.gps.course,   packet.gps.speed
                            );
                            }
                            Err(_) => {
                                info!("Bad packet at page {}, skipping", i);
                            }
                        }
                    }

                    info!("Flash dump complete.");
                    let _ = class.write_packet(b"--- end of dump ---\r\n").await;

                    // 3) Clear both header sectors so next run starts fresh
                    flash.erase_sector(0).await.unwrap();
                    flash.erase_sector(SECTOR_SIZE as u32).await.unwrap();
                    let _ = class.write_packet(b"Log cleared, ready for next session\r\n").await;
                }
            }
        }
    };

    // Run both the USB device and the echo task concurrently.
    join(usb_fut, echo_fut).await;
}

