#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 3;
float serialoutarray[3];

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{
  //Make dummy data
    for (int idx = 0;idx<numVars;idx++) {
		    serialoutarray[idx] = (idx+1)*sin((1.0/numVars)*3.141592654*(idx+1)*millis()/1000.0);
    }
    //Send data to processing
    serialPrintFloatArr(serialoutarray,numVars);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
}
