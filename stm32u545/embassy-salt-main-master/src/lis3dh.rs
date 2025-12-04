// Registers
pub const CTRL_REG0: u8 = 0x1E;
pub const CTRL_REG1: u8 = 0x20;
pub const CTRL_REG4: u8 = 0x23;
pub const CTRL_REG5: u8 = 0x24;
pub const LIS3DH_STATUS_REG: u8 = 0x27;
const OUT_X_L: u8 = 0x28;
pub const START_REG: u8 = OUT_X_L | 0x80;

// Commands
pub const LIS3DH_POWER_DOWN: u8 = 0b0000_0000; // power down
pub const LIS3DH_200Hz: u8 = 0b0110_0000;
pub const LIS3DH_400Hz: u8 = 0b0111_0000;
pub const LIS3DH_1344Hz: u8 = 0b1001_0000;
pub const LIS3DH_BDU: u8 = 0b1000_0000;
pub const LIS3DH_16G: u8 = 0b0011_0000;
pub const LIS3DH_HIGH_RES: u8 = 0b0000_1000;
pub const LIS3DH_BOOT: u8 = 0b1000_0000;
pub const LIS3DH_DATA_RDY: u8 = 0b0000_1000;
pub const LIS3DH_SA0_PU_DISABLE: u8 = 0b1000_0000;

// low and high addresses
pub const LIS3DH_LOW_ADDRESS: u8 = 0x18;
pub const LIS3DH_HIGH_ADDRESS: u8 = 0x19;

// scale and shift type
pub const LIS3DH_SCALE_16G: f32 = 0.012;  // g/LSB for 16G scale
pub const LIS3DH_SHIFT_HR: u8 = 4;  // when in HR mode (12 bit), shift 4

