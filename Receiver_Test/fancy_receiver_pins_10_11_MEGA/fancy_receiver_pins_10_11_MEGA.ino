#include <Servo.h>

Servo servo1;

unsigned long current_time,timer;
byte last_channel;
int pulse_time;

void setup() {
  // put your setup code here, to run once:
  servo1.attach(3);
  Serial.begin(9600);

  //For the Arduino UNO we can do this 
  //PCICR |= (1 << PCIE2); //PCIE2 is for pins 0-7
  //PCMSK2 |= (1 << PCINT19); //This is for pin 3 
  //PCMSK2 |= (1 << PCINT21); //This is for pin 5

  // or this
  //PCICR |= (1 << PCIE0); //This is for pins 8-13
  //PCMSK0 |= (1 << PCINT0); //This is pin 8
  //PCMSK0 |= (1 << PCINT1); //This is pin 9
  //For the Arduino MEGA we have to do this
  PCICR |= (1 << PCIE0); //PORTB 0-7
  //PORTB0 = Dpin 53 - PCINT0
  //PORTB1 = Dpin 52 - PCINT1
  //PORTB2 = Dpin 51 - PCINT2
  //PORTB3 = Dpin 50 - PCINT3
  //PORTB4 = Dpin 10 - PCINT4
  //PORTB5 = Dpin 11 - PCINT5
  //PORTB6 = Dpin 12 - PCINT6
  //PORTB7 = Dpin 13 - PCINT7
  PCMSK0 |= (1 << PCINT4); //PORTB4 - Digital Pin 10
  PCMSK0 |= (1 << PCINT5); //PORTB5 - Digital Pin 11
  PCMSK0 |= (1 << PCINT6); //PORTB6 - Digital Pin 12
  PCMSK0 |= (1 << PCINT7); //PORTB7 - Digital Pin 13
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //This is to debug pulses
  //0 deg = 521 ms
  //180 deg = 2300 ms
  servo1.write(0);
  delay(500);
  servo1.write(90);
  delay(500);
  servo1.write(180);
  delay(500);

  //Get pulse using Pulsein - comment out if you're using
  //interrupts
  //pulse_time = pulseIn(13,HIGH);
  //Serial.println(pulse_time);
  
}

//ISR(PCINT2_vect){ //This is for pins 3 and 5 on the UNO
//ISR(PCINT0_vect){ //This is for pins 8 and 9 on the UNO or PORTB (pins 10-13)
ISR(PCINT0_vect) { //Pins 10 and 11 for MEGA

  current_time = micros();

  //if (PIND & B00001000) { //This is pin 3 UNO
  //if (PINB & B00000001) { //This is pin 8 UNO
  //if (PIND & B00100000){ //This is pin 5 UNO
  //if (PINB & B00000010){ //This is pin 9 UNO
  //if (PINB & B00010000) { // PORTB4 is Digital Pin 10 MEGA
  //if (PINB & B00100000) { //PORTB5 is Digital Pin 11 MEGA
  //if (PINB & B01000000) { //PORTB6 is Digital Pin 12 MEGA
  //if (PINB & B10000000) { //PORTB7 is Digital Pin 13 MEGA
  
  if (PINB & B10000000) { 
  
    if(last_channel == 0)
    {
      last_channel = 1;
      timer = current_time;
    }
  }
  else if(last_channel == 1){
      last_channel = 0;
      pulse_time = current_time - timer;
      Serial.println(pulse_time);
  }

}
