#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);
 
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}
 
void loop(void) 
{
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  //imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  int8_t temp = bno.getTemp();
  
  /* Display the floating point data */
  Serial.print("Heading: ");
  Serial.print(euler.x());
  Serial.print(" Pitch: ");
  Serial.print(euler.y());
  Serial.print(" Roll: ");
  Serial.print(euler.z());
  Serial.print(" R: ");
  Serial.print(rate.z());
  Serial.print(" Q: ");
  Serial.print(rate.y());
  Serial.print(" P: ");
  Serial.print(rate.x());
//  Serial.print(" Ax: ");
//  Serial.print(accel.x());
//  Serial.print(" Ay: ");
//  Serial.print(accel.y());
//  Serial.print(" Az: ");
//  Serial.print(accel.z());
  Serial.print(" Lin Ax: ");
  Serial.print(lin_accel.x());
  Serial.print(" Lin Ay: ");
  Serial.print(lin_accel.y());
  Serial.print(" Lin Az: ");
  Serial.print(lin_accel.z());
  Serial.print(" Temp (C): ");
  Serial.print(temp);
  Serial.print("\n");
  
  delay(100);
}
