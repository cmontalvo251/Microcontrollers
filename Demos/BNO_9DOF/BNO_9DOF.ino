//This will simply print the Euler angles and rates to Serial. 

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() 
{
  Serial.begin(9600);
  
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
   imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
   imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
   imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
   int8_t temp = bno.getTemp();
  
   //SERIAL PRINTS
   //IMU Data
   Serial.print(euler.x()); Serial.print(" ");
   Serial.print(euler.y()); Serial.print(" ");
   Serial.print(euler.z()); Serial.print(" ");
   Serial.print(rate.x()); Serial.print(" ");
   Serial.print(rate.y()); Serial.print(" ");
   Serial.print(rate.z()); Serial.print(" ");
   Serial.print(lin_accel.x()); Serial.print(" ");
   Serial.print(lin_accel.y()); Serial.print(" ");
   Serial.print(lin_accel.z()); Serial.print(" ");
   Serial.print(temp); Serial.print(" "); 
   Serial.print("\n"); 
  
}
