//#include <Wire.h>
//#include "CommunicationUtils.h"

int numVars = 1; //Make sure this is the same as processing code
float data[1]; //You must hardcode the number of variables.

//Yellow is GND
//Orange is 5V
//

// Set the pins used
#define ledPin 13
//In an effort to test the sensors do not use 
//0 or 1 - TX/RX
//4,7 or 8 - GPS Shield
//10 - SD Card

//Your options are 2,3 // 5,6 // 9,11
#define frontTrigPin 11
#define frontEchoPin 10

//#define leftTrigPin 38
//#define leftEchoPin 40

//#define rightTrigPin 36
//#define rightEchoPin 37

long cmright,cmleft,cmfront;
//unsigned int cmleft;

void setup() {
  
  //Just need 9600 for prox sensors
  Serial.begin(9600);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
  pinMode(ledPin, OUTPUT);
  
  //Setup input and output
  //SetupPin(leftTrigPin,leftEchoPin);
  SetupPin(frontTrigPin,frontEchoPin);
  //SetupPin(rightTrigPin,rightEchoPin);
  
  Serial.println("Ready!");
}

void SetupPin(int TrigPin,int EchoPin) {
 pinMode(TrigPin,OUTPUT);
 pinMode(EchoPin,INPUT); 
}

long Distance(int TrigPin,int EchoPin) {
  long duration; 
  digitalWrite(TrigPin,LOW);
  delayMicroseconds(2); 
  digitalWrite(TrigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin,LOW);
  duration = pulseIn(EchoPin,HIGH);
  long cm = (duration/2.0)/29.1;  
  if (cm > 100) {
    cm = 100;
  }
  return cm;
}  

void loop() {
  //Here is where we will put the Prox Code
  //Serial.println("Pinging Proximity Sensors");
  //Writing just the current time
  //Serial.println("Writing Time");
  delay(50);

  //data[1] = Distance(rightTrigPin,rightEchoPin);
  //data[0] = Distance(leftTrigPin,leftEchoPin);
  data[0] = Distance(frontTrigPin,frontEchoPin);

  Serial.print(millis());
  Serial.print(" ");
  //Serial.print(data[1]);
  //Serial.print(" ");
  Serial.print(data[0]);
  //Serial.print(" ");
  //Serial.print(data[2]);
  Serial.print("\n"); 

  //serialPrintFloatArr(data,numVars);
  //Serial.println("\r\n");  

}


/* End code */

