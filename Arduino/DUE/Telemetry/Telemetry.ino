#include "telemetry.h"

Telemetry Xbee;

void setup()
{
  Xbee.begin(&Serial2);
}

//Just grab time_now and send to Xbee. Obviously need to send it data as well.
void loop()
{
  //Take Data
  float time_now = millis()/1000.0;
  
  float phi = 0.1*time_now;
  float theta = -0.2*time_now;
  float psi = 0.5*time_now;
  //Load into out vector
  Xbee.serialoutarray[0] = time_now;
  Xbee.serialoutarray[1] = phi;
  Xbee.serialoutarray[2] = theta;
  Xbee.serialoutarray[3] = psi; 
  
  Xbee.write(time_now);  
}
