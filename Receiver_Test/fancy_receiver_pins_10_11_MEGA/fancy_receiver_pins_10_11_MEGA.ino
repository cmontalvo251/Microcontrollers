#include <Servo.h>

Servo servo1,servo2,servo3;

unsigned long current_time,timer,timer2,timer3;
byte last_channel,last_channel2,last_channel3;
int pulse_time,pulse_time2,pulse_time3;
int numprint=0;

void setup() {
  // put your setup code here, to run once:
  servo1.attach(8); 
  servo2.attach(9); 
  servo3.attach(10); 
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
  //PCMSK0 |= (1 << PCINT4); //PORTB4 - Digital Pin 10
  PCMSK0 |= (1 << PCINT5); //PORTB5 - Digital Pin 11
  PCMSK0 |= (1 << PCINT6); //PORTB6 - Digital Pin 12
  PCMSK0 |= (1 << PCINT7); //PORTB7 - Digital Pin 13

  //Looks like Port 8 and 9 are PH6 and PH5 which is PORTH6 and PORTH5
  //We need to find which PCIE port handles portH
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //This is to debug pulses
  //0 deg = 521 ms
  //180 deg = 2300 ms
  Serial.print("FIRST \n");
  
  servo1.write(0);
  servo2.write(90);
  servo3.write(180);
  
  delay(2000);

  Serial.print("SECOND \n");
  
  servo1.write(90);
  servo2.write(180);
  servo3.write(0);
  
  delay(2000);

  Serial.print("THIRD \n");
  servo1.write(180);
  servo2.write(0);
  servo3.write(90);
  
  delay(2000);

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

  if (numprint > 2) {
    numprint = 0;
    Serial.print("\n");
  }

  //?Reading from pin 13
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
      //Serial.print("ONE:");
      Serial.print(pulse_time);
      Serial.print(" ");
      numprint++;
  }

  //Reading from pin 12
  if (PINB & B01000000) { 
  
    if(last_channel2 == 0)
    {
      last_channel2 = 1;
      timer2 = current_time;
    }
  }
  else if(last_channel2 == 1){
      last_channel2 = 0;
      pulse_time2 = current_time - timer2;
      //Serial.print("TWO:");
      Serial.print(pulse_time2);
      Serial.print(" ");
      numprint++;
  }

 //Reading from pin 11
 if (PINB & B00100000) { 
  
    if(last_channel3 == 0)
    {
      last_channel3 = 1;
      timer3 = current_time;
    }
  }
  else if(last_channel3 == 1){
      last_channel3 = 0;
      pulse_time3 = current_time - timer3;
      //Serial.print("THREE:");
      Serial.print(pulse_time3);
      Serial.print(" ");
      numprint++;
  }

}
