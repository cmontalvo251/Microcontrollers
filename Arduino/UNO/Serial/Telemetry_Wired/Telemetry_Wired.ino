#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 4;
String receive_message;
float serialoutarray[4];
float time_now;

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{
  //Only read if you've got adequate data
  //No need to perform a serialEvent because we don't need multiple processes here. We
  //literally just need to perform some calculations on the Arduino and send it back.
  if (Serial.available()) { 
    receive_message = Serial.readStringUntil('\n');
    time_now = millis()/1000.0;
    float phi = 0.1*time_now;
    float theta = -0.2*time_now;
    float psi = 0.5*time_now;
    serialoutarray[0] = time_now;
    serialoutarray[1] = phi;
    serialoutarray[2] = theta;
    serialoutarray[3] = psi; 
    serialPrintFloatArr(serialoutarray,numVars);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
  }
}