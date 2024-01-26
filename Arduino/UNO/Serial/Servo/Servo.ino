#include <Servo.h>
Servo servo1;

unsigned long zero_timer;

char val; //Data received from the serial port
int ival; //data converted to int
float degree; //Degree sent to servo

void setup() {
 Serial.begin(9600);  
 servo1.attach(6);
 zero_timer = micros();
 degree = 0;
}

void loop() {  
  degree += 10;
  if (degree > 180){ 
    degree = 0;
  }
  Serial.print(degree);
  servo1.write(degree);
  Serial.println("-----");
  delay(100);
} 
