#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 3;
float serialinarray[3];
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
  if (Serial.available()) { 
    String message = Serial.readStringUntil('\n');
    parseMessage(message);
    for (int idx = 0;idx < 3;idx++){
      serialoutarray[idx] = serialinarray[idx];
    }
    serialPrintFloatArr(serialoutarray,numVars);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
  }
}

void parseMessage(String inmessage) {
  String current_val;
  int ctr = 0;
  for (int idx = 0;idx<inmessage.length();idx++){
    if (inmessage[idx] == ',') {
      serialinarray[ctr] = current_val.toFloat();
      current_val = "";
      ctr++;
    }
    else {
      current_val+=inmessage[idx];
    }
  }
  serialinarray[2] = current_val.toFloat(); //Need to get the last one
}

