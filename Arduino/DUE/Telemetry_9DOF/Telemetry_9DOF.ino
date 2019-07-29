#include "telemetry.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Telemetry Xbee;

Adafruit_BNO055 bno;

void setup()
{
  ///Setup telemetry
  Xbee.begin(&Serial2);

  ///Setup the 9DOF
  bno = Adafruit_BNO055(55);

  Serial.begin(9600); //For eerror codes.

  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR! \n");
    while(1);
  } else {
    Serial.print("BNO Init Successful \n");
  }

  bno.setExtCrystalUse(true);
}

//Just grab time_now and send to Xbee. Obviously need to send it data as well.
void loop()
{
  //Take Data
  float time_now = millis()/1000.0;

  //Get roll pitch yaw
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float phi = PI/180.0*(euler.y()); 
  float theta = PI/180.0*(euler.z()-9); //The 9.0 is because the sensor is not mounted flat on the PCB
  float psi = PI/180.0*euler.x();
  
  //Load into out vector
  Xbee.serialoutarray[0] = time_now;
  Xbee.serialoutarray[1] = phi;
  Xbee.serialoutarray[2] = theta;
  Xbee.serialoutarray[3] = psi; 
  
  Xbee.write(time_now);  
}
