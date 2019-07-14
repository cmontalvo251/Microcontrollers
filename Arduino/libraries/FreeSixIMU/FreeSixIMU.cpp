/*
FreeSixIMU.cpp - A libre and easy to use orientation sensing library for Arduino
Copyright (C) 2011 Fabio Varesano <fabio at varesano dot net>

Development of this code has been supported by the Department of Computer Science,
Universita' degli Studi di Torino, Italy within the Piemonte Project
http://www.piemonte.di.unito.it/


This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <inttypes.h>
//#define DEBUG
#include "FreeSixIMU.h"
// #include "WireUtils.h"
//#include "DebugUtils.h"

//Ok here is the constructor 
FreeSixIMU::FreeSixIMU() {
  //this is a 6DOF sensor so the sensor has an accelerometer and a gyro
  acc = ADXL345();   //both of these classes have their respective header files
  gyro = ITG3200(); //and correspond to the sensors on the sensor shield
  //magn = HMC58X3(); //I'm guessing if you get 

  //the constructor for acc sets error codes and gains
  //the constructor for gyro set gains, offsets and RevPolarity
	
  // initialize quaternion
  q0 = 1.0f;
  q1 = 0.0f;
  q2 = 0.0f;
  q3 = 0.0f;
  exInt = 0.0;
  eyInt = 0.0;
  ezInt = 0.0;
  twoKp = twoKpDef; //these are proportional gains. Not sure what they are for
  twoKi = twoKiDef; //integral gain
  integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;
  lastUpdate = 0;
  now = 0;
}

void FreeSixIMU::init() {
  //The accelerometer is connected to hex address 0x53
  //The rate gyro is connected to hex address 0x68
  //the first two variables are addresses to the accelerometer
  //and the rate gyro. The third variable is fastmode which in this 
  //case is set to false.
  init(FIMU_ACC_ADDR, FIMU_ITG3200_DEF_ADDR, false);
}

void FreeSixIMU::init(bool fastmode) {
  init(FIMU_ACC_ADDR, FIMU_ITG3200_DEF_ADDR, fastmode);
}

void FreeSixIMU::init(int acc_addr, int gyro_addr, bool fastmode) {
  //Ok so this is the FreeSixIMU init routine which takes the acc and gyro
  //adress
  //Interesting, delay is written again. Perhaps just a check to make
  //sure things don't go too fast
  delay(5);
  
  //The ATMEGA is an Arduino chip. It looks like the Wire.h protocal
  //messes with the shield and some ports need to be disabled
  // disable internal pullups of the ATMEGA which Wire enable by default
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    // deactivate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    cbi(PORTC, 4);
    cbi(PORTC, 5);
  #else
    // deactivate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    cbi(PORTD, 0);
    cbi(PORTD, 1);
  #endif
  
  if(fastmode) { // switch to 400KHz I2C - eheheh
    TWBR = ((16000000L / 400000L) - 16) / 2; // see twi_init in Wire/utility/twi.c
    // TODO: make the above usable also for 8MHz arduinos..
    //See - fastmode does not actually do anything
  }
  
  // init ADXL345
  //Alright here is where we initialize the accelerometer
  //Really this just turns the unit on
  acc.init(acc_addr);

  // init ITG3200
  //Again here we turn the gyro on and then set a bunch
  //of hex address to variables that control the thingy
  gyro.init(gyro_addr);
  
  delay(1000); //this is a 1 second delay right? 1000 ms = 1 second? Yes, milli = 10^3
  //deci = 10^-1, centi = 10^-2, milli = 10^-3, micro = 10^-6, nano = 10^-9, pico = 10^-12
  // calibrate the ITG3200
  gyro.zeroCalibrate(128,5); 
  //The first variable is the total number of samples
  //so the gyro samples 128 times in this case
  //the second variable is the delay seconds
  //so the code waits 5 ms between measurements
  //the zerocalibrate takes 128 measurements and averages them to make 
  //an offset or bias measurement.
  
  //This is the magnetometer which is currently disabled.
  // init HMC5843
  //magn.init(false); // Don't set mode yet, we'll do that later on.
  // Calibrate HMC using self test, not recommended to change the gain after calibration.
  //magn.calibrate(1); // Use gain 1=default, valid 0-7, 7 not recommended.
  // Single mode conversion was used in calibration, now set continuous mode
  //magn.setMode(0);
  //delay(10);
  //magn.setDOR(B110);
  
}


void FreeSixIMU::getRawValues(int * raw_values) {
  acc.readAccel(&raw_values[0], &raw_values[1], &raw_values[2]);
  gyro.readGyroRaw(&raw_values[3], &raw_values[4], &raw_values[5]);
  //magn.getValues(&raw_values[6], &raw_values[7], &raw_values[8]);
  
}


void FreeSixIMU::getValues(float * values) {  
  int accval[3];
  //this calls the readAccel code with 3 function inputs
  acc.readAccel(&accval[0], &accval[1], &accval[2]);
  values[0] = ((float) accval[0]);
  values[1] = ((float) accval[1]);
  values[2] = ((float) accval[2]);
  //Ok so we've read the accel data and it looks like the accel data 
  //is in int so we convert it to a float

  gyro.readGyro(&values[3]); //Read the calibrated values and then
  //convert them to degrees per second
  
  //magn.getValues(&values[6]);
}


// Quaternion implementation of the 'DCM filter' [Mayhony et al].  Incorporates the magnetic distortion
// compensation algorithms from Sebastian Madgwick filter which eliminates the need for a reference
// direction of flux (bx bz) to be predefined and limits the effect of magnetic distortions to yaw
// axis only.
//
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
//=====================================================================================================
void FreeSixIMU::AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
  float recipNorm;
  float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
  float halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
  float qa, qb, qc;

  // Auxiliary variables to avoid repeated arithmetic
  q0q0 = q0 * q0;
  q0q1 = q0 * q1;
  q0q2 = q0 * q2;
  q0q3 = q0 * q3;
  q1q1 = q1 * q1;
  q1q2 = q1 * q2;
  q1q3 = q1 * q3;
  q2q2 = q2 * q2;
  q2q3 = q2 * q3;
  q3q3 = q3 * q3;
  
  
  /*
  // Use magnetometer measurement only when valid (avoids NaN in magnetometer normalisation)
  if((mx != 0.0f) && (my != 0.0f) && (mz != 0.0f)) {
    float hx, hy, bx, bz;
    float halfwx, halfwy, halfwz;
    
    // Normalise magnetometer measurement
    recipNorm = invSqrt(mx * mx + my * my + mz * mz);
    mx *= recipNorm;
    my *= recipNorm;
    mz *= recipNorm;
    
    // Reference direction of Earth's magnetic field
    hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
    hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
    bx = sqrt(hx * hx + hy * hy);
    bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));
    
    // Estimated direction of magnetic field
    halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
    halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
    halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);
    
    // Error is sum of cross product between estimated direction and measured direction of field vectors
    halfex = (my * halfwz - mz * halfwy);
    halfey = (mz * halfwx - mx * halfwz);
    halfez = (mx * halfwy - my * halfwx);
  }
  */

  //THis whole area above is just commented because it's not used in the 6DOF sensor

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if((ax != 0.0f) && (ay != 0.0f) && (az != 0.0f)) {
    float halfvx, halfvy, halfvz;
    
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;
    
    // Estimated direction of gravity
    halfvx = q1q3 - q0q2; //this is from the rotation matrix - I can get my students to derive this 
    halfvy = q0q1 + q2q3; //in class I think.
    halfvz = q0q0 - 0.5f + q3q3;
  
    // Error is sum of cross product between estimated direction and measured direction of field vectors
    halfex += (ay * halfvz - az * halfvy);
    halfey += (az * halfvx - ax * halfvz);
    halfez += (ax * halfvy - ay * halfvx);
  }
  
  //Ok so we are measuring the ax,ay,az and then integrating gx,gy,gz

  // Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
  if(halfex != 0.0f && halfey != 0.0f && halfez != 0.0f) {
    // Compute and apply integral feedback if enabled
    if(twoKi > 0.0f) {
      integralFBx += twoKi * halfex * (1.0f / sampleFreq);  // integral error scaled by Ki
      integralFBy += twoKi * halfey * (1.0f / sampleFreq);
      integralFBz += twoKi * halfez * (1.0f / sampleFreq);
      gx += integralFBx;  // apply integral feedback
      gy += integralFBy;
      gz += integralFBz;
    }
    else {
      integralFBx = 0.0f; // prevent integral windup
      integralFBy = 0.0f;
      integralFBz = 0.0f;
    }

    // Apply proportional feedback
    gx += twoKp * halfex;
    gy += twoKp * halfey;
    gz += twoKp * halfez;
  }
  
  // Integrate rate of change of quaternion
  gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
  gy *= (0.5f * (1.0f / sampleFreq));
  gz *= (0.5f * (1.0f / sampleFreq));
  qa = q0;
  qb = q1;
  qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz); //I just looked this up
  q1 += (qa * gx + qc * gz - q3 * gy); //this is the derivatives of a quaternion using p,q,r
  q2 += (qa * gy - qb * gz + q3 * gx); //This is pretty cool that they have this all here
  q3 += (qa * gz + qb * gy - qc * gx);
  
  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm; 
  q3 *= recipNorm;
}


void FreeSixIMU::getQ(float * q) {
  float val[9];
  getValues(val); //Read the raw sensor measurements from
  //the accel and gyro
  //now the val values have the accelerometer ax,ay,ax and gyro readings 
  //p,q,r
  
  /*
  DEBUG_PRINT(val[3] * M_PI/180);
  DEBUG_PRINT(val[4] * M_PI/180);
  DEBUG_PRINT(val[5] * M_PI/180);
  DEBUG_PRINT(val[0]);
  DEBUG_PRINT(val[1]);
  DEBUG_PRINT(val[2]);
  DEBUG_PRINT(val[6]); // these are magnetometer readings which are not used
  DEBUG_PRINT(val[7]);
  DEBUG_PRINT(val[8]);
  */
  
  
  now = micros(); //the number of seconds since the last function call to getQ
  sampleFreq = 1.0 / ((now - lastUpdate) / 1000000.0); //this is setting up the filter
  lastUpdate = now;
  // gyro values are expressed in deg/sec, the * M_PI/180 will convert it to radians/sec - nice comment
  //AHRSupdate(val[3] * M_PI/180, val[4] * M_PI/180, val[5] * M_PI/180, val[0], val[1], val[2], val[6], val[7], val[8]);
  // use the call below when using a 6DOF IMU - this is because 6,7,8 are magnetometer readings and they are not used in
  // the 6DOF sensor
  AHRSupdate(val[3] * M_PI/180, val[4] * M_PI/180, val[5] * M_PI/180, val[0], val[1], val[2], 0, 0, 0);
  q[0] = q0;
  q[1] = q1;
  q[2] = q2;
  q[3] = q3;
}

// Returns the Euler angles in radians defined with the Aerospace sequence.
// See Sebastian O.H. Madwick report 
// "An efficient orientation filter for inertial and intertial/magnetic sensor arrays" Chapter 2 Quaternion representation
void FreeSixIMU::getEuler(float * angles) {
float q[4]; // quaternion
  getQ(q);
  angles[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1) * 180/M_PI; // psi
  angles[1] = -asin(2 * q[1] * q[3] + 2 * q[0] * q[2]) * 180/M_PI; // theta
  angles[2] = atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1) * 180/M_PI; // phi
}


void FreeSixIMU::getAngles(float * angles) {
  float a[3]; //Euler
  getEuler(a);

  angles[0] = a[0];
  angles[1] = a[1];
  angles[2] = a[2];
  
  if(angles[0] < 0)angles[0] += 360;
  if(angles[1] < 0)angles[1] += 360;
  if(angles[2] < 0)angles[2] += 360;

}





void FreeSixIMU::getYawPitchRoll(float * ypr) {
  float q[4]; // quaternion
  float gx, gy, gz; // estimated gravity direction
  getQ(q);
  
  gx = 2 * (q[1]*q[3] - q[0]*q[2]);
  gy = 2 * (q[0]*q[1] + q[2]*q[3]);
  gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
  
  ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1) * 180/M_PI;
  ypr[1] = atan(gx / sqrt(gy*gy + gz*gz))  * 180/M_PI;
  ypr[2] = atan(gy / sqrt(gx*gx + gz*gz))  * 180/M_PI;
}


float invSqrt(float number) {
  volatile long i;
  volatile float x, y;
  volatile const float f = 1.5F;

  x = number * 0.5F;
  y = number;
  i = * ( long * ) &y;
  i = 0x5f375a86 - ( i >> 1 );
  y = * ( float * ) &i;
  y = y * ( f - ( x * y * y ) );
  return y;
}
