#include "telemetry.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_MPL115A2.h>

Telemetry Xbee;
Adafruit_MPL115A2 mpl115a2;
float getnewpressure = 0;
float pressure = 0;

void setup()
{
  ///Setup telemetry
  Xbee.begin(&Serial2);

  //Start the Pressure Sensor
  mpl115a2.begin();

  Serial.begin(9600); //For eerror codes.

  Serial.print("Pressure Sensor Init \n");

}

//Just grab time_now and send to Xbee. Obviously need to send it data as well.
void loop()
{
  //Take Data
  float time_now = millis()/1000.0;

  if ((time_now - getnewpressure) > 1.0) {
    pressure = mpl115a2.getPressure();
    getnewpressure = time_now;
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.print("\n");
  }

  //Load into out vector
  Xbee.serialoutarray[0] = time_now;
  Xbee.serialoutarray[1] = pressure;
  
  Xbee.write(time_now);  
}
