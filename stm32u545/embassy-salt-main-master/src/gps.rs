use core::cell::RefCell;
use defmt::*;
use core::fmt::Write as _;
use embassy_stm32::gpio::{Flex, Output, Pull};
use embassy_stm32::mode::{Async};
use embassy_stm32::usart::{UartRx, UartTx};
use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::channel::Channel;
use embassy_time::Timer;
use heapless::{String, Vec};
use embassy_futures::select::{select, Either, Select};
use embassy_sync::mutex::Mutex;
use serde::{Deserialize, Serialize};

// A static shared variable that holds the latest GPS update protected by a mutex.
// We use CriticalSectionRawMutex because your GPS task and aggregator run on the same executor.
pub static LATEST_GPS: Mutex<CriticalSectionRawMutex, RefCell<GpsRmc>> = Mutex::new(RefCell::new(DEFAULT_GPS_RMC));

// PMTK messages
pub const PMTK_RMC_ONLY: &str = "PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";  // only output RMC
pub const PMTK_SET_POS_FIX_10HZ: &str = "PMTK220,100";  // increase output rate to 10HZ
pub const PMTK_API_SET_FIX_CTL_10HZ: &str = "PMTK300,100,0,0,0,0";  // increase fix calculation to 10HZ

#[derive(Debug, Format, Clone, Default, Serialize, Deserialize)]
pub struct GpsRmc {
    /// UTC time as hhmmss (fractional seconds dropped), e.g., 123519.
    pub utc_time: u32,
    /// Date as ddmmyy, e.g., 230394.
    pub date: u32,
    /// Data validity: 'A' = valid, 'V' = invalid.
    pub status: u8,
    /// Latitude in signed decimal degrees (positive for North, negative for South).
    pub latitude: f32,
    /// Longitude in signed decimal degrees (positive for East, negative for West).
    pub longitude: f32,
    /// Speed over ground in knots.
    pub speed: f32,
    /// Course over ground in degrees.
    pub course: f32,
}
// Default GPS RMC initialization values
const DEFAULT_GPS_RMC: GpsRmc = GpsRmc {
    utc_time: 0,
    date: 0,
    status: b'V',    // For example, using b'V' to indicate an invalid status.
    latitude: 0.0,
    longitude: 0.0,
    speed: 0.0,
    course: 0.0,
};

/// Convert a raw GPS value in ddmm.mmmm (or dddmm.mmmm) format
/// into decimal degrees. This function handles negative values
/// correctly by extracting the absolute value first, then reapplying the sign.
pub fn convert_to_decimal(raw: f32) -> f32 {
    // Work with the absolute value first
    let raw_abs = raw.abs();
    // For positive numbers, truncating by casting to u32 gives the floor.
    let degrees = (raw_abs / 100.0) as u32 as f32;
    let minutes = raw_abs - (degrees * 100.0);
    let decimal = degrees + (minutes / 60.0);
    // Reapply original sign: if raw was negative, make the decimal negative.
    if raw < 0.0 {
        -decimal
    } else {
        decimal
    }
}

/// Parse the UTC time from a string of the form "hhmmss.sss".
/// Fractional seconds are dropped; e.g. "123519.00" becomes 123519.
fn parse_time_simple(s: &str) -> Option<u32> {
    // Split at the decimal and keep the integer part only.
    let int_part = s.split('.').next()?;
    int_part.parse::<u32>().ok()
}

/// Parse the date from a string "ddmmyy" into a u32.
fn parse_date_simple(s: &str) -> Option<u32> {
    s.parse::<u32>().ok()
}

// ==============================
// RMC Parsing Function (Minimal)
// ==============================

/// Parses an RMC sentence string and returns a GpsRmc struct.
/// Returns None if the sentence is not a valid RMC sentence.
fn parse_rmc(sentence: &str) -> Option<GpsRmc> {
    // Remove checksum (if it exists) by splitting at '*'
    let sentence_no_checksum = sentence.split('*').next()?;

    // Only process RMC sentences (variants may be "$GPRMC" or "$GNRMC")
    if !sentence_no_checksum.starts_with("$GPRMC") && !sentence_no_checksum.starts_with("$GNRMC") {
        return None;
    }

    // Split the sentence into its fields.
    let parts: heapless::Vec<&str, 16> = sentence_no_checksum.split(',').collect();
    if parts.len() < 12 {
        return None;
    }

    // Process each field, using default values if needed:
    let utc_time = if !parts[1].is_empty() {
        parse_time_simple(parts[1]).unwrap_or_default()
    } else {
        0
    };

    let status = parts[2].bytes().next().unwrap_or(b'V');

    // Convert latitude:
    let latitude_raw = if !parts[3].is_empty() {
        parts[3].parse::<f32>().ok().unwrap_or_default()
    } else {
        0.0
    };

    let latitude = if let Some(dir) = parts[4].chars().next() {
        let conv = convert_to_decimal(latitude_raw);
        if dir == 'S' { -conv } else { conv }
    } else {
        0.0
    };

    // Convert longitude:
    let longitude_raw = if !parts[5].is_empty() {
        parts[5].parse::<f32>().ok().unwrap_or_default()
    } else {
        0.0
    };

    let longitude = if let Some(dir) = parts[6].chars().next() {
        let conv = convert_to_decimal(longitude_raw);
        if dir == 'W' { -conv } else { conv }
    } else {
        0.0
    };

    let speed = if !parts[7].is_empty() {
        parts[7].parse::<f32>().ok().unwrap_or_default()
    } else {
        0.0
    };

    let course = if !parts[8].is_empty() {
        parts[8].parse::<f32>().ok().unwrap_or_default()
    } else {
        0.0
    };

    let date = if !parts[9].is_empty() {
        parse_date_simple(parts[9]).unwrap_or_default()
    } else {
        0
    };

    Some(GpsRmc {
        utc_time,
        date,
        status,
        latitude,
        longitude,
        speed,
        course,
    })
}

#[embassy_executor::task]
pub async fn gps_reader(
    mut rx: UartRx<'static, Async>
) {
    let mut buf = [0u8; 128];
    // Heapless buffer to accumulate NMEA data.
    let mut line = Vec::<u8, 512>::new();

    loop {
        match rx.read_until_idle(&mut buf).await {
            Ok(n) => {
                for &byte in &buf[..n] {
                    if line.push(byte).is_err() {
                        line.clear();
                        warn!("GPS buffer overflow - clearing data");
                    }
                    if byte == b'\n' {
                        if let Ok(sentence) = str::from_utf8(&line) {
                            let trimmed = sentence.trim_end();
                            //info!("GPS RX: {}", trimmed);
                            if let Some(rmc) = parse_rmc(trimmed) {
                                //info!("Parsed RMC: {:?}", rmc);
                                // Update the shared latest GPS data.
                                let gps_lock = LATEST_GPS.lock().await;
                                *gps_lock.borrow_mut() = rmc;
                            } else {
                                warn!("Received non-RMC or invalid sentence");
                            }
                        } else {
                            warn!("GPS RX: Invalid UTF-8");
                        }
                        line.clear();
                    }
                }
            }
            Err(e) => {
                warn!("GPS RX error: {:?}", e);
                line.clear();
            }
        }
    }
}

pub async fn pulse_gps_standby(mut stby_pin: Flex<'_>) {
    info!("Pulsing GPS STANDBY pin low to exit standby");
    // Step 1: Pull low
    stby_pin.set_low();
    Timer::after_millis(150).await;

    // Step 2: Release to a float
    stby_pin.set_as_input(Pull::None);

    // Step 3: Now the pin is floating (Hi-Z), GPS will go to Full On
    info!("Released STANDBY pin (floating now, GPS should wake up)");
}

pub async fn setup_gps(
    tx: &mut UartTx<'static, Async>,
    rx: &mut UartRx<'static, Async>,
    gps_stby: Flex<'_>,
) {
    // ensure not in standby mode
    pulse_gps_standby(gps_stby).await;
    
    // enable RTC only
    if let Err(e) = send_pmtk_cmd_and_wait_ack(tx, rx, PMTK_RMC_ONLY).await {
        // Handle the error, e.g., log it or take corrective action
        info!("Error: {}", e);
    }
    
    // increase message output interval
    if let Err(e) = send_pmtk_cmd_and_wait_ack(tx, rx, PMTK_SET_POS_FIX_10HZ).await {
        // Handle the error, e.g., log it or take corrective action
        info!("Error: {}", e);
    }
    
    // increase fix calculation to match
    if let Err(e) = send_pmtk_cmd_and_wait_ack(tx, rx, PMTK_API_SET_FIX_CTL_10HZ).await {
        // Handle the error, e.g., log it or take corrective action
        info!("Error: {}", e);
    }
}

/// Calculate NMEA checksum (XOR of all bytes between '$' and '*')
fn calculate_checksum(sentence: &str) -> u8 {
    sentence.bytes().fold(0u8, |acc, b| acc ^ b)
}

/// Sends a PMTK command and waits for an ACK response.
pub async fn send_pmtk_cmd_and_wait_ack(
    tx: &mut UartTx<'static, Async>,
    rx: &mut UartRx<'static, Async>,
    command_body: &str,
) -> Result<(), &'static str> {
    let checksum = calculate_checksum(command_body);

    let mut full_msg: String<128> = String::new();
    core::write!(full_msg, "${}*{:02X}\r\n", command_body, checksum).map_err(|_| "fmt err")?;

    // Send the command byte-by-byte
    for b in full_msg.as_bytes() {
        tx.write(&[*b]).await.map_err(|_| "TX failed")?;
    }

    //info!("Sent: {}", full_msg.trim());

    let mut buf = [0u8; 1];
    let mut response: String<128> = String::new();

    let cmd_number = command_body
        .split(',')
        .next()
        .and_then(|s| s.strip_prefix("PMTK"))
        .ok_or("Invalid command")?;

    
    loop {
        // Set a timeout duration for waiting for the ACK (e.g., 1 second)
        let timeout = Timer::after_millis(1000); // Adjust this duration if needed
        
        match select(rx.read(&mut buf), timeout).await {
            Either::First(Ok(_)) => {
                let c = buf[0] as char;
                if response.push(c).is_err() {
                    return Err("ACK overflow");
                }

                if response.ends_with("\r\n") && response.contains("$PMTK001") {
                    let line = response.trim();
                    if line.contains(cmd_number) {
                        //defmt::info!("Received ACK: {}", line);
                        return Ok(());
                    } else {
                        defmt::warn!("Unrelated ACK: {}", line);
                        response.clear();
                    }
                }

                if response.len() > 120 {
                    return Err("ACK overflow");
                }
            },
            Either::First(Err(_)) => return Err("RX failed"),
            Either::Second(_) => return Err("ACK timeout"),
        }
    }
}