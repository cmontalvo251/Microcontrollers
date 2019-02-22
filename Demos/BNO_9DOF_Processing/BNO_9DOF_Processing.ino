////This will send Euler Angle data to Processing
#include "CommunicationUtils.h" 
#include <Wire.h>

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

float PQR[3];
float Euler[3];
float Qout[4];

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  
  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
    
  bno.setExtCrystalUse(true);
}

void loop() {
  
   //Grab the 9DOF Data
   //imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
   //imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
   imu::Quaternion quats = bno.getQuat();
   
//   Euler[0] = euler.x();
//   Euler[1] = euler.y();
//   Euler[2] = euler.z();
//   PQR[0] = rate.x();
//   PQR[1] = rate.y();
//   PQR[2] = rate.z();
    Qout[0] = quats.w();
    Qout[1] = quats.x();
    Qout[2] = quats.y();
    Qout[3] = quats.z();
  
   //Send Euler Angles to Computer
   //serialPrintFloatArr(Euler,3);
   //Send PQR to Computer
   //serialPrintFloatArr(PQR,3);
   serialPrintFloatArr(Qout,4);
   Serial.println("\r\n"); //line break

   delay(60); //Delay for 60 milliseconds so we don't send a shit ton of information
   //to the computer.
  
}
