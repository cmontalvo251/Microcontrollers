//Normally to control a servo you #include the Servo.h library
//#include <Servo.h>

//And then declare a servo using the Servo class defined in servo1.
//Servo servo1;

//In order to keep track of the loop timer we need an unsigned long for the loop and the servo
unsigned long zero_timer,servo_timer;

void setup() {
  //In the setup routine you "Attach a servo to a certain pin". In this case Pin 6
  //Which is a PWM (Pulse Width Modulation) port. Note also that although you can plug
  //a servo into any pin make sure to use a PWM port if you are using the PWM library. 
  //The reason being because erroneous things can happen if you don't use a PWM port.
  //servo1.attach(6);
  
  //Now if we don't want to use the standard Servo library things become a bit more
  //complex. First we need to configure pin 6 to be an output. Pins 0-7 are in portD
  //Thus we need to set pin 6 of portD to high or a 1. This is the equivalent to
  //Servo.attach(6) or more simply pinMode(6,OUTPUT).
  DDRD |= B01000000;
  
  //We also need to have the Arduino run in Interrupt mode so that it runs faster
  PCICR |= (1 << PCIE0);
  
  //Furthermore we want to have the loop run for a certain number of seconds so we initialize
  //zero timer
  zero_timer = micros();
}

void loop() {
  //In the loop we simply use the write() function to write a degree between 0 and 180.
  //servo1.write(45);
  
  //Instead of using the Servo library we are going to force the loop to run at 4000us
  while(zero_timer + 4000 > micros());
  
  //Once the while loop breaks out we reset the zero_timer
  zero_timer = micros();
  
  //Now we can generate our PWM signal. A PWM signal is LOW (0) HIGH (1) wait 1000-2000us LOW(1)
  //So first we need to set pin 6 of PORTD to HIGH (1)
  PORTD |= B01000000;
  //PORTD = PORTD | B0100000;
  //Then we wait anything between 500-3500us; 
  delayMicroseconds(3500);
  //And finally we set pin 6 of PORTD to LOW (0)
  PORTD &= B10111111;
  //PORTD = PORTD & B10111111;
}
