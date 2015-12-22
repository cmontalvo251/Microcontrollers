/****************************************************************************
* ITG3200.cpp - ITG-3200/I2C library v0.5 for Arduino                         *
* Copyright 2010-2011 Filipe Vieira & various contributors                  *
* http://code.google.com/p/itg-3200driver                                   *
* This file is part of ITG-3200 Arduino library.                            *
*                                                                           *
* This library is free software: you can redistribute it and/or modify      *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
****************************************************************************/
/****************************************************************************
* Tested on Arduino Mega with ITG-3200 Breakout                             *
* SCL     -> pin 21     (no pull up resistors)                              *
* SDA     -> pin 20     (no pull up resistors)                              *
* CLK & GND -> pin GND                                                      *
* INT       -> not connected  (but can be used)                             *
* VIO & VDD -> pin 3.3V                                                     *
*****************************************************************************/
#include "FIMU_ITG3200.h"


ITG3200::ITG3200() {
  setGains(1.0,1.0,1.0); //this just sets the variables gains[3]
  setOffsets(0.0,0.0,0.0); //offsets[3]
  setRevPolarity(0,0,0); //polarities[3]
  //Wire.begin();       //Normally this code is called from setup() at user code
                        //but some people reported that joining I2C bus earlier
                        //apparently solved problems with master/slave conditions.
                        //Uncomment if needed.
}

void ITG3200::init(unsigned int  address) {
  // Uncomment or change your default ITG3200 initialization

  //It looks like we can initialize our rate gyro in 4 different ways
  
  // fast sample rate - divisor = 0 filter = 0 clocksrc = 0, 1, 2, or 3  (raw values)
  init(address, NOSRDIVIDER, RANGE2000, BW256_SR8, PLL_XGYRO_REF, true, true);
  
  // slow sample rate - divisor = 0  filter = 1,2,3,4,5, or 6  clocksrc = 0, 1, 2, or 3  (raw values)
  //init(address,NOSRDIVIDER, RANGE2000, BW010_SR1, INTERNALOSC, true, true);
  
  // fast sample rate 32Khz external clock - divisor = 0  filter = 0  clocksrc = 4  (raw values)
  //init(address,NOSRDIVIDER, RANGE2000, BW256_SR8, PLL_EXTERNAL32, true, true);
  
  // slow sample rate 32Khz external clock - divisor = 0  filter = 1,2,3,4,5, or 6  clocksrc = 4  (raw values)
  //init(address,NOSRDIVIDER, RANGE2000, BW010_SR1, PLL_EXTERNAL32, true, true);

}

//Our variables are the following

//address = the rate gryo hex address on the 6DOF shield

//_SRateDiv = The sample rate divider - FsampleHz=SampleRateHz/(divider+1)
//If divider is zero the FsampleHz is equal to SampleRateHz

//_Range = the range of the gyro right now it is set to 3 I'll learn about this in setFSRange

//_filterBW = This is the ditigal Low Pass Filter Bandwidth. Rate gyros are super noisy so you need to filter
//the signal - the setting are numbers from 0-6. Again take a look at setFilterBW to learn more

//_ClockSrc = Another number from 0-6. Learn more in setClockSource

//The last two variables are always true in the init routine. They are booleans and used to set
//setITGReady(true) and setRawDataReady(true)

void ITG3200::init(unsigned int address, byte _SRateDiv, byte _Range, byte _filterBW, byte _ClockSrc, bool _ITGReady, bool _INTRawDataReady) {
  _dev_address = address; //set the input of the function to the private variable hex address
  setSampleRateDiv(_SRateDiv); //set the _SRateDiv variable to the address of the SMPLRT_DIV hex address
  setFSRange(_Range); //the the DLPF_FS address to a 1 or a 0 because of logic
  setFilterBW(_filterBW); //set the address to 1 or a 0
  setClockSource(_ClockSrc); //set the address to a 1 or a 0 
  setITGReady(_ITGReady); // << I think the << symbol is actually a bit shift operator.
  setRawDataReady(_INTRawDataReady); //ok yes so the numbers 0 through 6 represent 000 - 110 in binary
  //these numbers are shifted a certain number of bits to shift to a different number
  //in addition, a & b is actually a bitwise operator rather than a standard or so 
  // 101 & 100 = 100 rather than 5 && 3 = 1 < true or false. Interesting, so all these
  // set commands are way more complicated than I thought.
  delay(GYROSTART_UP_DELAY);  // startup - wait 70 ms to make sure everything is good to go
  //there is a lot of waiting and initialization protocol in this code here
}

byte ITG3200::getDevAddr() {
  /*readmem(WHO_AM_I, 1, &_buff[0]); 
  return _buff[0];  */
  return _dev_address;
}

void ITG3200::setDevAddr(unsigned int  _addr) {
  writemem(WHO_AM_I, _addr); 
  _dev_address = _addr;
}

byte ITG3200::getSampleRateDiv() {
  readmem(SMPLRT_DIV, 1, &_buff[0]);
  return _buff[0];
}

void ITG3200::setSampleRateDiv(byte _SampleRate) {
  writemem(SMPLRT_DIV, _SampleRate);
}

byte ITG3200::getFSRange() {
  readmem(DLPF_FS, 1, &_buff[0]);
  return ((_buff[0] & DLPFFS_FS_SEL) >> 3);
}

void ITG3200::setFSRange(byte _Range) { ///So _Range is currently set to 3
  readmem(DLPF_FS, 1, &_buff[0]);  //so we read what is current in the buffer
  writemem(DLPF_FS, ((_buff[0] & ~DLPFFS_FS_SEL) | (_Range << 3)) ); //then we write a 1 or a 0 depending
  //on this true falst statement
}

byte ITG3200::getFilterBW() {   ///It doesn't look like this routine is called in this cpp 
  readmem(DLPF_FS, 1, &_buff[0]); //file. Maybe it's used somewhere else?
  return (_buff[0] & DLPFFS_DLPF_CFG); 
}

void ITG3200::setFilterBW(byte _BW) {   
  readmem(DLPF_FS, 1, &_buff[0]); //Again wierd. _BW can be between 0 and 6
  writemem(DLPF_FS, ((_buff[0] & ~DLPFFS_DLPF_CFG) | _BW)); //then we write a 0 or 1 to the memory location. Again not really sure on this one
}

bool ITG3200::isINTActiveOnLow() {  
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_ACTL) >> 7);
}

void ITG3200::setINTLogiclvl(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_ACTL) | (_State << 7))); 
}

bool ITG3200::isINTOpenDrain() {  
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_OPEN) >> 6);
}

void ITG3200::setINTDriveType(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_OPEN) | _State << 6)); 
}

bool ITG3200::isLatchUntilCleared() {    
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_LATCH_INT_EN) >> 5);
}

void ITG3200::setLatchMode(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_LATCH_INT_EN) | _State << 5)); 
}

bool ITG3200::isAnyRegClrMode() {    
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_INT_ANYRD_2CLEAR) >> 4);
}

void ITG3200::setLatchClearMode(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_INT_ANYRD_2CLEAR) | _State << 4)); 
}

bool ITG3200::isITGReadyOn() {   
  readmem(INT_CFG, 1, &_buff[0]);
  return ((_buff[0] & INTCFG_ITG_RDY_EN) >> 2);
}

void ITG3200::setITGReady(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_ITG_RDY_EN) | _State << 2)); 
}

bool ITG3200::isRawDataReadyOn() {
  readmem(INT_CFG, 1, &_buff[0]);
  return (_buff[0] & INTCFG_RAW_RDY_EN);
}

void ITG3200::setRawDataReady(bool _State) {
  readmem(INT_CFG, 1, &_buff[0]);
  writemem(INT_CFG, ((_buff[0] & ~INTCFG_RAW_RDY_EN) | _State)); 
}

bool ITG3200::isITGReady() {
  readmem(INT_STATUS, 1, &_buff[0]);
  return ((_buff[0] & INTSTATUS_ITG_RDY) >> 2);
}

bool ITG3200::isRawDataReady() {
  readmem(INT_STATUS, 1, &_buff[0]);
  return (_buff[0] & INTSTATUS_RAW_DATA_RDY);
}

void ITG3200::readTemp(float *_Temp) {
  readmem(TEMP_OUT,2,_buff);
  *_Temp = 35 + (((_buff[0] << 8) | _buff[1]) + 13200) / 280.0;    // F=C*9/5+32  
}

void ITG3200::readGyroRaw(int *_GyroX, int *_GyroY, int *_GyroZ){
  readmem(GYRO_XOUT, 6, _buff); //This reads 6 bytes 
  //into the data buffer _buff
  //This part below is what is beyond me.
  //Basically it says the pointer of GyroX is the bit wise operation
  //from _buff[0] shifted to the left 8 places then bitwise or with _buff[1]
  //Again not really sure how that works but that's how they have it set up
  //Ok so I learned this from the accelerometer 
  //but basically the sensors are 10 bit sensors and 
  //the buffer is an integer or 32 bits so the process
  //below converts the readings to integers
  *_GyroX = ((_buff[0] << 8) | _buff[1]);
  *_GyroY = ((_buff[2] << 8) | _buff[3]); 
  *_GyroZ = ((_buff[4] << 8) | _buff[5]);
}

void ITG3200::readGyroRaw(int *_GyroXYZ){
  readGyroRaw(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2);
}

void ITG3200::setRevPolarity(bool _Xpol, bool _Ypol, bool _Zpol) {
  polarities[0] = _Xpol ? -1 : 1;
  polarities[1] = _Ypol ? -1 : 1;
  polarities[2] = _Zpol ? -1 : 1;
}

void ITG3200::setGains(float _Xgain, float _Ygain, float _Zgain) {
  gains[0] = _Xgain;
  gains[1] = _Ygain;
  gains[2] = _Zgain;
}

void ITG3200::setOffsets(int _Xoffset, int _Yoffset, int _Zoffset) {
  offsets[0] = _Xoffset;
  offsets[1] = _Yoffset;
  offsets[2] = _Zoffset;
}

void ITG3200::zeroCalibrate(unsigned int totSamples, unsigned int sampleDelayMS) {
  int xyz[3]; 
  float tmpOffsets[] = {0,0,0};

  for (int i = 0;i < totSamples;i++){
    delay(sampleDelayMS); 
    readGyroRaw(xyz);
    tmpOffsets[0] += xyz[0];
    tmpOffsets[1] += xyz[1];
    tmpOffsets[2] += xyz[2];  
  }
  setOffsets(-tmpOffsets[0] / totSamples, -tmpOffsets[1] / totSamples, -tmpOffsets[2] / totSamples);
  //so bascially this code reads the gyro value 128 times and then averages the measurements
  //to create an offset
}

void ITG3200::readGyroRawCal(int *_GyroX, int *_GyroY, int *_GyroZ) {
  readGyroRaw(_GyroX, _GyroY, _GyroZ);
  *_GyroX += offsets[0];
  *_GyroY += offsets[1];
  *_GyroZ += offsets[2];
}

void ITG3200::readGyroRawCal(int *_GyroXYZ) {
  readGyroRawCal(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2);
}

void ITG3200::readGyro(float *_GyroX, float *_GyroY, float *_GyroZ){
  int x, y, z;
  //Here we read the raw data but calibrated with the offsets obtained from
  //the init routine where we measure 128 measurements to remove the bias
  
  readGyroRawCal(&x, &y, &z); // x,y,z will contain calibrated integer values from the sensor
  //Typically polarities are all 1
  //and the gain values are also 1
  //Ok so wht's 14.375?
  //45/pi = 14.324? Maybe it's some type of conversion?
  //Nope - just checked the data sheet
  //14.375 is a conversion from bits to degrees per second
  //the unit is LSB or least significant bit thus when 1 bit increases
  //the reading goes up by 14.375 degrees/second
  *_GyroX =  x / 14.375 * polarities[0] * gains[0];
  *_GyroY =  y / 14.375 * polarities[1] * gains[1];
  *_GyroZ =  z / 14.375 * polarities[2] * gains[2];
}

void ITG3200::readGyro(float *_GyroXYZ){
  readGyro(_GyroXYZ, _GyroXYZ+1, _GyroXYZ+2);
}

void ITG3200::reset() {     
  writemem(PWR_MGM, PWRMGM_HRESET); 
  delay(GYROSTART_UP_DELAY); //gyro startup 
}

bool ITG3200::isLowPower() {   
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_SLEEP) >> 6;
}
  
void ITG3200::setPowerMode(bool _State) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & ~PWRMGM_SLEEP) | _State << 6));  
}

bool ITG3200::isXgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_XG) >> 5;
}

bool ITG3200::isYgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_YG) >> 4;
}

bool ITG3200::isZgyroStandby() {
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_STBY_ZG) >> 3;
}

void ITG3200::setXgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_XG) | _Status << 5));
}

void ITG3200::setYgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_YG) | _Status << 4));
}

void ITG3200::setZgyroStandby(bool _Status) {
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & PWRMGM_STBY_ZG) | _Status << 3));
}

byte ITG3200::getClockSource() {  
  readmem(PWR_MGM, 1, &_buff[0]);
  return (_buff[0] & PWRMGM_CLK_SEL);
}

void ITG3200::setClockSource(byte _CLKsource) {   
  readmem(PWR_MGM, 1, &_buff[0]);
  writemem(PWR_MGM, ((_buff[0] & ~PWRMGM_CLK_SEL) | _CLKsource)); 
}

void ITG3200::writemem(uint8_t _addr, uint8_t _val) {
  //This must have been written by two different people but with similar
  //interests. This is pretty annoying. 
  //writemem is the same as writeTo
  Wire.beginTransmission(_dev_address);   // start transmission to device 
  Wire.write(_addr); // send register address
  Wire.write(_val); // send value to write
  Wire.endTransmission(); // end transmission
}

void ITG3200::readmem(uint8_t _addr, uint8_t _nbytes, uint8_t __buff[]) {
  Wire.beginTransmission(_dev_address); // start transmission to device 
  Wire.write(_addr); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(_dev_address); // start transmission to device 
  Wire.requestFrom(_dev_address, _nbytes);// send data n-bytes read
  uint8_t i = 0; 
  while (Wire.available()) {
    __buff[i] = Wire.read(); // receive DATA
    i++;
  }
  Wire.endTransmission(); // end transmission
}

