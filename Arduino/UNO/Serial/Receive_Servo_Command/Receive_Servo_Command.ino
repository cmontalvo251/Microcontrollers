#include <Servo.h>
Servo servo1;

extern float ReceiveNumber(void);
float df = 0;
char val; //Data received from the serial port
int ival; //data converted to int
float degree; //Degree sent to servo

void setup() {
 Serial.begin(57600);  
 servo1.attach(6);
 pinMode(LED_BUILTIN,OUTPUT);
 digitalWrite(LED_BUILTIN,LOW);
 degree = 0;
 delay(1000);
}

void loop() {  
  digitalWrite(LED_BUILTIN,HIGH);
  df = ReceiveNumber();
  //delay(10);
  digitalWrite(LED_BUILTIN,LOW);
  //delay(10);
  //df = 1;
  degree = 0 + df*180.0;
  servo1.write(degree);
} 
