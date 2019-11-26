#include "CommunicationUtils.h" 
#include <Wire.h> //Send data to Processing

float volts[4];

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
  volts[3] = millis()/1000.0; //time

  if (true)  {
    serialPrintFloatArr(volts,4);
    Serial.println("\r\n"); //line break
  } else {
    Serial.print("T = ");
    Serial.print(volts[3]);
    Serial.print(" LBS = ");
    for (int i = 0 ; i < 3; i++) {
      Serial.print(volts[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
   
  delay(2000); //Send data every half a second
}
