// Register Addresses
pub const CTRL1_XL: u8 = 0x10;   // Accelerometer control register
pub const CTRL2_G: u8 = 0x11;    // Gyroscope control register
pub const CTRL3_C: u8 = 0x12;  // Control register 3 (contains SW_RESET)
pub const CTRL9_XL: u8 = 0x18;  // Control register 9
pub const I3C_BUS_AVB: u8 = 0x62;  // I3C AVB register
pub const OUTX_L_A: u8 = 0x28;   // Accelerometer X-axis low byte
pub const OUTX_L_G: u8 = 0x22;   // Gyroscope X-axis low byte
pub const LSMDSOX_STATUS_REG: u8 = 0x1E;      // Status register to check if data is ready
pub const LSM6DSOX_ADDRESS: u8 = 0x6A;  // LSM6DSOX when SA0 is low

pub enum AccelerometerScale {
    /// ±2g accelerometer Scale
    Accel2g = 0b0000_0000,
    /// ±16g accelerometer Scale
    ///
    /// When ```XL_FS_MODE``` in ```CTRL8_XL``` is set to 1, ```FS_XL_16g``` sets scale to 2g.
    Accel16g = 0b0000_0100,
    /// ±4g accelerometer Scale
    Accel4g = 0b0000_1000,
    /// ±8g accelerometer Scale
    Accel8g = 0b0000_1100,
}

impl AccelerometerScale {
    /// Returns the factor needed to convert a [i16] of the specified range to a float.
    ///
    /// Multiplying a given [i16] with the corresponding factor, will yield the measured g-force.
    pub fn to_factor(&self) -> f32 {
        match self {
            AccelerometerScale::Accel2g => 0.000061,
            AccelerometerScale::Accel16g => 0.000488,
            AccelerometerScale::Accel4g => 0.000122,
            AccelerometerScale::Accel8g => 0.000244,
        }
    }
}

pub enum GyroscopeScale {
    /// ±125dps gyroscope scale
    Dps125 = 0b0000_0010,
    /// ±250dps gyroscope scale
    Dps250 = 0b0000_0000,
    /// ±500dps gyroscope scale
    Dps500 = 0b0000_0100,
    /// ±1000dps gyroscope scale
    Dps1000 = 0b0000_1000,
    /// ±2000dps gyroscope scale
    Dps2000 = 0b0000_1100,
}

impl GyroscopeScale {
    /// Returns the factor needed to convert a [i16] of the specified range to a float.
    ///
    /// Note: The values have been copied from the official ST driver.
    /// They correspond nicely to the scarce examples from the application note,
    /// but otherwise don't make much sense (for example calculate `125d/0x7FFF` and you'll get `0.003814`, slightly different than `0.004375`).
    /// On the other hand factors used for [lsm6dsox::AccelerometerScale] seem to be correct in the official ST driver and can be calculated as shown above.
    ///
    /// Also see this [GitHub issue](https://github.com/STMicroelectronics/lsm6dsox/issues/2).
    pub fn to_factor(&self) -> f32 {
        match self {
            GyroscopeScale::Dps125 => 0.004375,
            GyroscopeScale::Dps250 => 0.008750,
            GyroscopeScale::Dps500 => 0.01750,
            GyroscopeScale::Dps1000 => 0.0350,
            GyroscopeScale::Dps2000 => 0.070,
        }
    }
}

pub enum DataRate {
    /// Power down state
    PowerDown = 0b0000_0000,
    /// 1.6 Hz
    ///
    /// Only available for Accelerometer.
    /// *"Low power mode"* only, defaults to 12 Hz when in *"high performance mode"*.
    Freq1Hz6 = 0b1011_0000,
    /// 12.5 Hz
    ///
    /// (low power)
    Freq12Hz5 = 0b0001_0000,
    /// 26 Hz
    ///
    /// (low power)
    Freq26Hz = 0b0010_0000,
    /// 52 Hz
    ///
    /// (low power)
    Freq52Hz = 0b0011_0000,
    /// 104 Hz
    ///
    /// (normal mode)
    Freq104Hz = 0b0100_0000,
    /// 208 Hz
    ///
    /// (normal mode)
    Freq208Hz = 0b0101_0000,
    /// 416 Hz
    ///
    /// (high performance)
    Freq416Hz = 0b0110_0000,
    /// 833 Hz
    ///
    /// (high performance)
    Freq833Hz = 0b0111_0000,
    /// 1.66 kHz
    ///
    /// (high performance)
    Freq1660Hz = 0b1000_0000,
    /// 3.33 kHz
    ///
    /// (high performance)
    Freq3330Hz = 0b1001_0000,
    /// 6.66 kHz
    ///
    /// (high performance)
    Freq6660Hz = 0b1010_0000,
}

impl From<DataRate> for f32 {
    fn from(data_rate: DataRate) -> f32 {
        match data_rate {
            DataRate::PowerDown => 0.0,
            DataRate::Freq1Hz6 => 1.6,
            DataRate::Freq12Hz5 => 12.5,
            DataRate::Freq26Hz => 26.0,
            DataRate::Freq52Hz => 52.0,
            DataRate::Freq104Hz => 104.0,
            DataRate::Freq208Hz => 208.0,
            DataRate::Freq416Hz => 416.0,
            DataRate::Freq833Hz => 833.0,
            DataRate::Freq1660Hz => 1660.0,
            DataRate::Freq3330Hz => 3330.0,
            DataRate::Freq6660Hz => 6660.0,
        }
    }
}

