#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 3;
int serialinarray[3];
float serialoutarray[3];
int serialcount = 0;

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
  if (Serial.available() > 2) { //I think 2 means 2 integers either that or an int is actually 1 byte
    for (int idx = 0;idx<3;idx++) {
      serialinarray[idx] = Serial.read();
      //Serial.write(serialinarray[idx]);
      serialoutarray[idx] = float(serialinarray[idx])+0.2;
    }
    serialPrintFloatArr(serialoutarray,numVars);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
  }
}
