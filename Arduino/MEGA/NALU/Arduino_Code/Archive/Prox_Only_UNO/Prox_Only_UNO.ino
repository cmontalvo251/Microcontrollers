#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 3; //Make sure this is the same as processing code
float data[3]; //You must hardcode the number of variables.

//Yellow is GND
//Orange is 5V
//

// Set the pins used
#define ledPin 13
//In an effort to test the sensors do not use 
//0 or 1 - TX/RX
//4,7 or 8 - GPS Shield
//10 - SD Card

//Your options are 2,3 // 5,6 // 4,9
#define frontTrigPin 2
#define frontEchoPin 3

#define leftTrigPin 5
#define leftEchoPin 6

#define rightTrigPin 4
#define rightEchoPin 9


long cmright,cmleft,cmfront;
//unsigned int cmleft;

void setup() {
  
  PCICR |= (1 << PCIE0);
  
  Serial.begin(115200);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
  pinMode(ledPin, OUTPUT);
  
  //Setup input and output
  SetupPin(leftTrigPin,leftEchoPin);
  SetupPin(frontTrigPin,frontEchoPin);
  SetupPin(rightTrigPin,rightEchoPin);
  
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
  
  long duration; 

  //data[1] = Distance(rightTrigPin,rightEchoPin);
  
  //digitalWrite(4,LOW);
  PORTD &= B11101111;
  delayMicroseconds(2); 
  //digitalWrite(4,HIGH);
  PORTD |= B00010000;
  delayMicroseconds(10);
  PORTD &= B11101111;
  //digitalWrite(4,LOW);
  duration = pulseIn(9,HIGH);
  data[1] = (duration/2.0)/29.1;  
  if (data[1] > 100) {
    data[1] = 100;
  }  
  
  //data[0] = Distance(leftTrigPin,leftEchoPin);
  
  //digitalWrite(5,LOW);
  PORTD &= B11011111; //Pin 5
  delayMicroseconds(2); 
  //digitalWrite(5,HIGH);
  PORTD |= B00100000; //Pin 5
  delayMicroseconds(10);
  PORTD &= B11011111; //Pin 5
  //digitalWrite(5,LOW);
  duration = pulseIn(6,HIGH); //Pin 6
  data[0] = (duration/2.0)/29.1;  
  if (data[0] > 100) {
    data[0] = 100;
  }
  
  //data[2] = Distance(frontTrigPin,frontEchoPin);  
  
  //digitalWrite(2,LOW);
  PORTD &= B11111011;
  delayMicroseconds(2); 
  //digitalWrite(2,HIGH);
  PORTD |= B00000100;
  delayMicroseconds(10);
  PORTD &= B11111011;
  //digitalWrite(2,LOW);
  duration = pulseIn(3,HIGH);
  data[2] = (duration/2.0)/29.1;  
  if (data[2] > 100) {
    data[2] = 100;
  }

  Serial.print(millis());
  Serial.print(" ");
  Serial.print(data[1]);
  Serial.print(" ");
  Serial.print(data[0]);
  Serial.print(" ");
  Serial.print(data[2]);
  Serial.print("\n"); 

  //serialPrintFloatArr(data,numVars);
  //Serial.println("\r\n");  

}


/* End code */

