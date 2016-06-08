//#include <Servo.h>
//Servo servo1;

unsigned long zero_timer;

char val; //Data received from the serial port
int ival; //data converted to int
float microseconds; //Degree sent to servo

void setup() {
 Serial.begin(9600);  
 //servo1.attach(6);
 
  //Now if we don't want to use the standard Servo library things become a bit more
  //complex. First we need to configure pin 6 to be an output. Pins 0-7 are in portD
  //Thus we need to set pin 6 of portD to high or a 1. This is the equivalent to
  //Servo.attach(6) or more simply inMode(6,OUTPUT).
  DDRD |= B01000000;
  
  //We also need to have the Arduino run in Interrupt mode so that it runs faster
  PCICR |= (1 << PCIE0);
  
  //Furthermore we want to have the loop run for a certain number of seconds so we initialize
  //zero timer
  zero_timer = micros();
}

void loop() {
  
 //Instead of using the Servo library we are going to force the loop to run at 4000us
 while(zero_timer + 4000 > micros());
  
 if (Serial.available() > 0) {
  val = Serial.read();
  //val = '0';
  //Serial.println(val);
  ival = (int)val; //0 to int is 48 (ASCII), 9 in int is 57 (ASCII)
  //Serial.println(ival);
  //degree = map(ival,48,57,0,180);
  microseconds = map(ival,48,57,500,2500);
  //Serial.println(degree);
  //servo1.write(degree);
  //Serial.println("-----");
  
  //Once the while loop breaks out we reset the zero_timer
  zero_timer = micros();
  
  //Now we can generate our PWM signal. A PWM signal is LOW (0) HIGH (1) wait 1000-2000us LOW(1)
  //So first we need to set pin 6 of PORTD to HIGH (1)
  PORTD |= B01000000;
  //Then we wait anything between 500-3500us; 
  delayMicroseconds(microseconds);
  //And finally we set pin 6 of PORTD to LOW (0)
  PORTD &= B10111111;
 } 
}
