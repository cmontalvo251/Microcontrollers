#include <Wire.h>
#include "CommunicationUtils.h"

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
#define frontTrigPin 6
#define frontEchoPin 7

long cmfront;
float servo_deg,servo_deg_old;
//unsigned int cmleft;

#include <Servo.h>

Servo motion;

void setup() {
  
  //Just need 9600 for prox sensors
  Serial.begin(9600);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
  pinMode(ledPin, OUTPUT);
  
  //Setup input and output
  SetupPin(frontTrigPin,frontEchoPin);

  motion.attach(8);
  
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
  
  data[0] = Distance(frontTrigPin,frontEchoPin);

  servo_deg = servo_deg_old * 0.6 + data[0]*180.0/100.0 * 0.4;
 
  motion.write(servo_deg);

  servo_deg_old = servo_deg;

  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.print(data[0]);
  Serial.print(" ");
  Serial.print(servo_deg_old);
  Serial.print(" ");
  Serial.print(servo_deg);
  Serial.print("\n"); 

  

  //serialPrintFloatArr(data,numVars);
  //Serial.println("\r\n");  

}


/* End code */

