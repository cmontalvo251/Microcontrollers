#include <Wire.h>
#include "CommunicationUtils.h"

int numControl = 3;
int NUMSTATES = 6;
float State[6]; //Expects phi,theta,psi p,q,r
float M_Control[3]; //Send M_Control LMN.
int serialcount = 0;
float Kp[3];
float ptpc[3];
float Kd[3];
float pqrc[3];

void setup()
{
  //Setup Control Gains
  //Initialize Control Variables
   for (int idx = 0;idx<3;idx++){
     Kp[idx] = 10.0;
     Kd[idx] = 9.0;
     pqrc[idx] = 0;
     ptpc[idx] = 0;
   }
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{

  //delay(100); //IF YOU MAKE THIS TOO BIG THE SATELLITE WON'T STABILIZE
  //Only read if you've got adequate data
  //No need to perform a serialEvent because we don't need multiple processes here. We
  //literally just need to perform some calculations on the Arduino and send it back.
  if (Serial.available()) { 
    String message = Serial.readStringUntil('\n');
    parseMessage(message);

    //Compute M_Control Based on State Vector
    Control();

    //Send it Back to Processing
    serialPrintFloatArr(M_Control,numControl);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
  }
}

void Control() {
  //Assume you are reading the Statevector perfectly for now
  //LMNC = Kp*(ptpc-ptp) + Kd*(pqrc-pqr);
  for (int idx = 0;idx < NUMSTATES;idx++){
      if (idx >= 3) {
        //These are angular rates so hit it with Kd
        M_Control[idx-3]+=Kd[idx-3]*(pqrc[idx-3]-State[idx]);
      }
      else {
        //These are euler angles so hit it with Kp
        M_Control[idx] = Kp[idx]*(ptpc[idx]-State[idx]);
      }
  }
}

void parseMessage(String inmessage) {
  String current_val;
  int ctr = 0;
  for (int idx = 0;idx<inmessage.length();idx++){
    if (inmessage[idx] == ',') {
      State[ctr] = current_val.toFloat();
      current_val = "";
      ctr++;
    }
    else {
      current_val+=inmessage[idx];
    }
  }
  State[NUMSTATES-1] = current_val.toFloat(); //Need to get the last one
}

