pub const IIS2MDCTR_ADDRESS: u8 = 0x1E;  // fixed since no SA0 pin

// Register addresses
pub const CFG_REG_A: u8 = 0x60;  // contains reboot, rst, ODR
pub const CFG_REG_C: u8 = 0x62;  // BDU, data ready
pub const STATUS_REG: u8 = 0x67;  // contains data ready information
pub const OUTX_L_REG: u8 = 0x68;  // the start of the data registers. write MSB to 1 to auto increment

// Commands
pub const IIS2MDCTR_TEMP_EN: u8 = 0b1000_0000;
pub const IIS2MDCTR_ODR_100Hz: u8 = 0b0000_1100;
pub const IIS2MDCTR_CONTINUOUS_MODE: u8 = 0b0000_0000;  // first two bits set to zero
pub const IIS2MDCTR_BDU_EN: u8 = 0b0001_0000;
pub const IIS2MDCTR_DATA_RDY: u8 = 0b0000_1000;
pub const BEGIN_READ_REG: u8 = OUTX_L_REG | 0x80;  // send this to auto increment

// Scales
pub const IIS2MDCTR_GAUSS_SCALE: f32 = 0.0015;  // gauss/LSB
