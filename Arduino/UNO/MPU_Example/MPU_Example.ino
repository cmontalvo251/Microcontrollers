// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t rAcX,rAcY,rAcZ,rTmp,rGyX,rGyY,rGyZ;
#define G 9.81

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  rAcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  rAcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  rAcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  rTmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  rGyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  rGyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  rGyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  //Conversions from Datasheet
  //MPU6050 has a built-in 16 bit ADC so the range is -2 to 2 G 
  //and the range in bits is 65536 so the resolution is 4 G / 65536 bits = 0.061 mg / bit
  //so bits / mg = 16.384 or bits/g = 16384
  //These are two's compliment ADCs so the first 0 to 32,000 bits are for positive and the 
  //32,000 to 65,000 are for negative values. The I2C converts it to a signed number though from +-32,000
  //Just to make the math easier for us.
  double AcX = G*rAcX/16384.;
  double AcY = G*rAcY/16384.;
  double AcZ = G*rAcZ/16384.;
  //The range on the gyro is +-250 degrees so that's 65536/500 = 131 bits / (deg/s)
  double GyX = rGyX/131;
  double GyY = rGyY/131;
  double GyZ = rGyZ/131;
  //I didn't look this one up because it was already coded in here
  double Tmp = rTmp/340. + 36.53; //In Celsius
  
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  delay(100);
}
