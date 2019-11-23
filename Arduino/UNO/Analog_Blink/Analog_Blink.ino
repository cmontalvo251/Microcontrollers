/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
const int outputpin = 9;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(9600);
  pinMode(outputpin, OUTPUT);  //Change the 7 to a 13 if you want the built in one
  Serial.print("SETUP!\n");
}

// the loop function runs over and over again forever
int value = 0;
void loop() {
  value+=10;
  analogWrite(outputpin, value);   // turn the LED on (HIGH is the voltage level)
  delay(100);              // wait for a second
  if (value > 255) {
    value = 0;
  }
  Serial.print(value);
  Serial.print(" ");
  Serial.print(value*5./255.);
  Serial.print("\n");
}
