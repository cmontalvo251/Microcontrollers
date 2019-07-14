#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 6; //Make sure this is the same as processing code
float data[6]; //You must hardcode the number of variables.

void setup() 
{
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
}


void loop()
{
  //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)

  //Generate Data
  for (int idx = 0;idx<numVars;idx++) {
    data[idx]=(idx+1)*sin((1.0/numVars)*3.141592654*(idx+1)*millis()/1000.0);
  }
  
  //Write Data to Serial
  serialPrintFloatArr(data,numVars);
  Serial.println("\r\n"); //line break. Tells processing to stop reading data

  //wait a bit so you don't write super freaking fast
  //delay(10);
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}

