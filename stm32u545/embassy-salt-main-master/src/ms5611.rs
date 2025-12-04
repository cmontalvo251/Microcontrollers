use cortex_m::prelude::{_embedded_hal_blocking_i2c_Write, _embedded_hal_blocking_i2c_WriteRead};
use defmt::*;
use embassy_embedded_hal::shared_bus::blocking::i2c::I2cDevice;
use embassy_stm32::i2c::I2c;
use embassy_stm32::mode::Async;
use embassy_sync::blocking_mutex::raw::NoopRawMutex;
use embassy_time::Timer;
use serde::{Deserialize, Serialize};

// address
pub const MS5611_ADDRESS: u8 = 0x77;  // when CSB is low

// commands
pub const RESET_CMD: u8 = 0x1E;
pub const PROM_READ_CMD: u8 = 0xA0;
pub const ADC_READ_CMD: u8 = 0x00;
pub const CONVERT_PRESSURE_CMD: u8 = 0x40;
pub const CONVERT_TEMP_CMD: u8 = 0x50;


// PROM calibration data struct
pub struct PromData {
    /// From datasheet, C1.
    pub pressure_sensitivity: u16,
    /// From datasheet, C2.
    pub pressure_offset: u16,
    /// From datasheet, C3.
    pub temp_coef_pressure_sensitivity: u16,
    /// From datasheet, C4.
    pub temp_coef_pressure_offset: u16,
    /// From datasheet, C5.
    pub temp_ref: u16,
    /// From datasheet, C6.
    pub temp_coef_temp: u16,
}

/// Oversampling ratio
/// See datasheet for more information.
pub enum Osr {
    Opt256,
    Opt512,
    Opt1024,
    Opt2048,
    Opt4096,
}

impl Osr {
    fn get_delay(&self) -> u64 {
        match *self {
            Osr::Opt256 => 1,
            Osr::Opt512 => 2,
            Osr::Opt1024 => 3,
            Osr::Opt2048 => 5,
            Osr::Opt4096 => 10,
        }
    }

    fn addr_modifier(&self) -> u8 {
        match *self {
            Osr::Opt256 => 0,
            Osr::Opt512 => 2,
            Osr::Opt1024 => 4,
            Osr::Opt2048 => 6,
            Osr::Opt4096 => 8,
        }
    }
}

#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct Ms5611Sample {
    // pressure measured in millibars
    pub pressure_mbar: f32,
    // temperature in degrees celcius
    pub temp_c: f32,
}

/// Triggers a hardware reset of the device.
pub async fn reset(i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>) -> Result<(), ()> {
    const MAX_TRIES: u8 = 5;
    for attempt in 1..=MAX_TRIES {
        match i2c.write(MS5611_ADDRESS, &[RESET_CMD]) {
            Ok(_) => {
                // give the MS5611 time to boot up
                Timer::after_millis(50).await;
                return Ok(());
            }
            Err(e) => {
                warn!("MS5611 reset attempt {}/{} failed: {:?}", attempt, MAX_TRIES, e);
                // small back-off before retrying
                Timer::after_millis(10).await;
            }
        }
    }
    error!("MS5611 reset failed after {} attempts", MAX_TRIES);
    Err(())
}

pub async fn read_prom(i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>) -> Result<PromData, ()> {
    let mut crc_check = 0u16;

    // This is the CRC scheme in the MS5611 AN520 (Application Note)
    fn crc_accumulate_byte(crc_check: &mut u16, byte: u8) {
        *crc_check ^= byte as u16;
        for _ in 0..8 {
            if (*crc_check & 0x8000) > 0 {
                *crc_check = (*crc_check << 1) ^ 0x3000;
            } else {
                *crc_check = *crc_check << 1;
            }
        }
    }

    fn crc_accumulate_buf2(crc_check: &mut u16, buf: &[u8]) {
        crc_accumulate_byte(crc_check,buf[0]);
        crc_accumulate_byte(crc_check,buf[1]);
    }

    let mut prom_raw = [0u16; 8];  // for parsing PromData result
    let mut buf = [0u8; 2];
    for i in 0..8 {
        let addr = PROM_READ_CMD + (i * 2);
        info!("addr: {:x}", addr);
        match i2c.write_read(MS5611_ADDRESS, &[addr], &mut buf) {
            Ok(_) => {info!("Prom Buff: {}, 0x{:x}", i, buf);}
            Err(_) => error!("Failed to read PROM snippet")
        }
        prom_raw[i as usize] = u16::from_be_bytes(buf);
        // Skip CRC accumulation for last entry (contains CRC itself)
        if i != 7 {
            crc_accumulate_buf2(&mut crc_check, &buf);
        }
    }
    
    // CRC is only last 4 bits
    let crc = u16::from_be_bytes(buf) & 0x000F;
    crc_accumulate_byte(&mut crc_check, buf[0]);
    crc_accumulate_byte(&mut crc_check, 0);

    crc_check = crc_check >> 12;

    defmt::assert_eq!(
        crc, crc_check,
        "PROM CRC did not match: expected {:X}, calculated {:X}",
        crc, crc_check
    );

    Ok(PromData {
        pressure_sensitivity: prom_raw[1],
        pressure_offset: prom_raw[2],
        temp_coef_pressure_sensitivity: prom_raw[3],
        temp_coef_pressure_offset: prom_raw[4],
        temp_ref: prom_raw[5],
        temp_coef_temp: prom_raw[6],
    })
}

/// Based on oversampling ratio, function may block between 1ms (OSR=256)
/// to 18ms (OSR=4096). To avoid blocking, consider invoking this function
/// in a separate thread.
pub async fn read_sample(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    prom: &PromData,
    osr: Osr,
) -> Result<Ms5611Sample, ()> {
    let mut adc_buf = [0u8; 3];  // buffer for adc reads

    // Start pressure conversion
    match i2c.write(MS5611_ADDRESS, &[CONVERT_PRESSURE_CMD + osr.addr_modifier()]) {
        Ok(()) => {}
        Err(_) => {error!("Failed to start pressure conversion");
            return Err(());
        }
    }
    Timer::after_millis(osr.get_delay()).await;

    // Read ADC result
    match i2c.write_read(MS5611_ADDRESS, &[ADC_READ_CMD], &mut adc_buf) {
        Ok(()) => {}
        Err(_) => {error!("Failed to read pressure ADC");
            return Err(());
        }
    }
    let d1 = i32::from_be_bytes([0, adc_buf[0], adc_buf[1], adc_buf[2]]);

    // Start temperature conversion
    match i2c.write(MS5611_ADDRESS, &[CONVERT_TEMP_CMD + osr.addr_modifier()]) {
        Ok(()) => {}
        Err(_) => {
            error!("Failed to start temperature conversion");
            return Err(());
        }
    }
    Timer::after_millis(osr.get_delay() as u64).await;

    // Read ADC result
    match i2c.write_read(MS5611_ADDRESS, &[ADC_READ_CMD], &mut adc_buf) {
        Ok(()) => {}
        Err(_) => {
            error!("Failed to read temperature ADC");
            return Err(());
        }
    }
    let d2 = i32::from_be_bytes([0, adc_buf[0], adc_buf[1], adc_buf[2]]) as i64;

    let dt = d2 - ((prom.temp_ref as i64) << 8);

    let mut temp = 2000 + (((dt * prom.temp_coef_temp as i64) >> 23) as i32);

    let mut offset = ((prom.pressure_offset as i64) << 16)
        + ((dt * prom.temp_coef_pressure_offset as i64) >> 7);
    let mut sens = ((prom.pressure_sensitivity as i64) << 15)
        + ((dt * prom.temp_coef_pressure_sensitivity as i64) >> 8);

    let mut t2 = 0i32;
    let mut off2 = 0i64;
    let mut sens2 = 0i64;
    
    // Second order temperature compensation
    // Low temperature (< 20C)
    if temp < 2000 {
        t2 = ((dt * dt) >> 31) as i32;
        off2 = ((5 * (temp - 2000).pow(2)) >> 1) as i64;
        sens2 = off2 >> 1;
        
        // Very low temperature (< -15)
        if temp < -1500 {
            off2 += 7 * (temp as i64 + 1500).pow(2);
            sens2 += (11 * (temp as i64 + 1500).pow(2)) >> 1;
        }
    }

    temp -= t2;
    offset -= off2;
    sens -= sens2;
    
    // Units: mbar * 100
    let pressure = (((((d1 as i64) * sens) >> 21) - offset) >> 15) as i32;

    Ok(Ms5611Sample {
        pressure_mbar: pressure as f32 / 100.0,
        temp_c: temp as f32 / 100.0,
    })
}