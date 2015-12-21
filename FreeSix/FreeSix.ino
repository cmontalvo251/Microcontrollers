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

// Set the FreeIMU object
FreeSixIMU my3IMU = FreeSixIMU(); //This class is defined in FreeSixIMU.h
//this FreeSixIMU class has acc and gyro classes in it.
//the constructor for this class initializes the acc and gyro and initializes
//all the variables it needs

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  //Normally this code is called from setup() at user code
  //but some people reported that joining I2C bus earlier
  //apparently solved problems with master/slave conditions.
  //Uncomment if needed (copied from line 35 of FIMU_ITG3200.cpp)

  delay(5); //pause for 5 milliseconds - again most likely tuned to make sure everything comes up 
  //correctly
  my3IMU.init(); //init with no arguments runs the init routine without fast mode
  //I honestly don't think fastmode is important to learn but I might read it later
  delay(5);
}


void loop() { 
  my3IMU.getQ(q);
  serialPrintFloatArr(q, 4);
  Serial.println(""); //line break
 
  delay(60);
}
