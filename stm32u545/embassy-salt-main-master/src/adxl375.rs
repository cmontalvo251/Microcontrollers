// Register addresses
pub const BW_RATE: u8 = 0x2C;  // bandwidth and ODR register
pub const FIFO_CTL: u8 = 0x38;  // FIFO control register
pub const INT_SOURCE: u8 = 0x30;  // read only register for interrupt events
pub const DATA_FORMAT: u8 = 0x31;  // right/left justify, invert, SPI, etc
pub const POWER_CTL: u8 = 0x2D;  // enable measure mode and control sleep in this register
pub const DATAX0: u8 = 0x32;  // starting location of accel data. goes to 0x37 for XYZ axis'
pub const OFSX: u8 = 0x1E;
pub const OFSY: u8 = 0x1F;
pub const OFSZ: u8 = 0x20;

// Output data rates
pub const ADXL375_200HZ: u8 = 0b0000_1011; // 200Hz ODR, 100Hz BW (maximum for 100kHz I2C)

// Commands
pub const ADXL375_MEASURE_MODE: u8 = 0b0000_1000; // command to enable measure mode
pub const ADXL375_FIFO_BYPASS: u8 = 0b0000_0000;
pub const ADXL375_FIFO_STREAM: u8 = 0b1000_0000;
pub const ADXL375_FIFO_ENABLE: u8 = 0b0100_0000;
pub const ADXL375_FIFO_TRIGGER: u8 = 0b1100_0000;
pub const ADXL375_JUSTIFY_RIGHT: u8 = 0b0000_0000; // mask of 0b0000_0100

// Scale factor
pub const ADXL375_SCALE_FACTOR: f32 = 0.049;  // G per LSB (49 mg per LSB)
pub const ADXL375_OFFSET_SCALE: f32 = 0.196;  // g per LSB

// low and high addresses
pub const ADXL375_LOW_ADDRESS: u8 = 0x53;
pub const ADXL375_HIGH_ADDRESS: u8 = 0x1D;

// Constants for sensor offsets (these would be set once after calibration)
pub const ADXL375_1_OFFSET_X: i16 = 0;
pub const ADXL375_1_OFFSET_Y: i16 = 0;
pub const ADXL375_1_OFFSET_Z: i16 = 5;

pub const ADXL375_2_OFFSET_X: i16 = 0;
pub const ADXL375_2_OFFSET_Y: i16 = 1;
pub const ADXL375_2_OFFSET_Z: i16 = 7;