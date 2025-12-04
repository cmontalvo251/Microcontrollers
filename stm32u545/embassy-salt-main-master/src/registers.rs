use cortex_m::prelude::{_embedded_hal_blocking_i2c_Write, _embedded_hal_blocking_i2c_WriteRead};
use defmt::*;
use embassy_stm32::i2c::I2c;
use embassy_embedded_hal::shared_bus::blocking::i2c::I2cDevice;
use embassy_stm32::mode::Async;
use embassy_sync::blocking_mutex::raw::NoopRawMutex;
use embassy_time::{Timer, Delay, Duration};
use {defmt_rtt as _, panic_probe as _};

/// Read-modify-write a register for a given sensor at a given address. Prevents overwriting
/// the already present register bits.
pub async fn update_reg_address(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8,
    reg: u8,
    bitmask: u8,
    new_value: u8,
) -> Result<(), ()> {
    let mut reg_val = [0u8; 1];

    // Step 1: Read the current value of the register
    match i2c.write_read(address, &[reg], &mut reg_val) {
        Ok(()) => {
            let old_val = reg_val[0];

            // Step 2: Modify the register value by clearing the bits specified by the mask
            reg_val[0] &= !bitmask; // Clear the bits we want to change

            // Step 3: Set the new data to the masked bits
            reg_val[0] |= new_value & bitmask; // Apply new value to the masked bits

            // Step 4: Log old and new register value
            info!("Register 0x{:02X}: old=0x{:02X}, new=0x{:02X}", reg, old_val, reg_val[0]);

            // Step 5: Write the modified value back to the register
            match i2c.write(address, &[reg, reg_val[0]]) {
                Ok(()) => {
                    info!("Register 0x{:02X} updated successfully", reg);
                    Ok(())
                },
                Err(_) => {
                    error!("Failed to write register 0x{:02X}", reg);
                    Err(())
                }
            }
        },
        Err(_) => {
            error!("Failed to read register 0x{:02X}", reg);
            Err(())
        }
    }
}

pub async fn debug_register(
    i2c: &mut I2cDevice<'static, NoopRawMutex, I2c<'static, Async>>,
    address: u8,
    register: u8,
    name: &str,
) {
    let mut buf = [0u8; 1];
    if i2c.write_read(address, &[register], &mut buf).is_ok() {
        info!("{} (0x{:02X}): 0x{:02X}", name, register, buf[0]);
    } else {
        error!("Failed to read {}", name);
    }
}
