#include "CommunicationUtils.h" 
#include <Wire.h> //Send data to Processing

float volts[3];

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  Wire.begin();
}

// the loop function runs over and over again forever
void loop() {
  volts[0] = analogRead(A0)*5./1023.; //load
  volts[1] = analogRead(A1)*5./1023.; //battery
  volts[2] = analogRead(A2)*5./1023.; //solar

  serialPrintFloatArr(volts,3);
  Serial.println("\r\n"); //line break
  
  delay(500); //Send data every half a second
}
