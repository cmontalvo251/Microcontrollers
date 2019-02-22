////////////////////////////////////////////////////////////
// Arduino firmware for use with FreeSixCube processing example
////////////////////////////////////////////////////////////

//Code downloaded from DFRduino
//Edited by Carlos Montalvo 12/20/2015

//In order to install the FreeSixIMU library you need to 
//click Sketch>Import Library...>Add Library and Click the ZIP file to add
//Once it's added though you should just be able to compile this code no problem.

#include <FreeSixIMU.h> //header file for Sixdof IMU code
#include <FIMU_ADXL345.h> //this is the accelerometer
#include <FIMU_ITG3200.h> //this is the rate gyro

#define DEBUG
#ifdef DEBUG
#include "DebugUtils.h" //Simple debug flags programs
#endif

#include "CommunicationUtils.h" //Looks like commands to write serially
#include <Wire.h> //Built in Arduino read and write buffer code


float q[4]; //hold q values 
float euler[3];
float val[9];
float outval[6];

// Set the FreeIMU object
FreeSixIMU my3IMU = FreeSixIMU(); //This class is defined in FreeSixIMU.h
//this FreeSixIMU class has acc and gyro classes in it.
//the constructor for this class initializes the acc and gyro and initializes
//all the variables it needs

void setup() {
  Serial.begin(115200); //REVIST - this needs to be the same as the processing code
  Wire.begin(); 
  //Normally this code is called from setup() at user code
  //but some people reported that joining I2C bus earlier
  //apparently solved problems with master/slave conditions.
  //Uncomment if needed (copied from line 35 of FIMU_ITG3200.cpp)

  delay(5); //pause for 5 milliseconds - again most likely tuned to make sure everything comes up 
  //correctly
  my3IMU.init(); //init with no arguments runs the init routine without fast mode
  //I honestly don't think fastmode is important to learn but I might read it later
  //ok so in the init routine we turn on the gyro and the accelerometer and calibrate
  //the gyro by taking 128 measurements and averaging all the measurements
  //all the filter and bandwidth parameters seem to be internal circuitry that operates
  //without code.
  delay(5);

  //That's it for setup
}


void loop() { 
  //Now we get the quaternions from the sensors
  //Ok I went through this code. The code really just takes the raw accel and gyro data
  //and integrates it using a DCM filter to get the quaternions
  //so an exercise would be to take the raw accel and gyro data and obtain the quaternions
  //using the method obtained here. That would be a fun project anyway.
  //Perhaps I could write my own routine where I output the raw accel and gyro 
  //data to a text file and then they obtain the quaternions
  my3IMU.getQ(q);
  my3IMU.getValues(val);
  
  //Debugging
  for (int idx = 0;idx<6;idx++)
  {
    outval[idx] = val[idx];
  }
  
  //val[4] = 1;
  //val[5] = -1;
  //q[1] = 0;
  //q[2] = 0;
  //q[3] += 0.1;

  //Serial.print(millis());
  //Serial.print(" "); 
  //for (int idx = 0;idx<4;idx++) {
  //   Serial.print(q[idx]);
  //   Serial.print(" ");
  //}

  //Convert to Euler
  //my3IMU.getEuler(euler);
  //for (int idx = 0;idx<3;idx++) {
  //  Serial.print(euler[idx]);
  //  Serial.print(" ");
  //}
  
  //Serial.println(); 
  
  //Ok then, you use the serialPrintFloatArr command from 
  //CommunicationUtils to send the quaternions to Processing
  //or really just over serial
  serialPrintFloatArr(q, 4); // -- quaternions is 4 numbers a float is 4 bytes so 4*4 = 16 bytes right?
  //this command is actually pretty neat and takes care of all
  //the byte operators and pointers and stuff. It would have taken
  //me forever to get this to work
  serialPrintFloatArr(outval,6);
  Serial.println("\r\n"); //line break
 
  delay(60);
}
