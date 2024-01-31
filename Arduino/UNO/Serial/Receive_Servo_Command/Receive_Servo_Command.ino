#include <Servo.h>
Servo servo1;

extern float ReceiveNumber(void);
char val; //Data received from the serial port
int ival; //data converted to int
float degree=0; //Degree sent to servo

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
  degree = ReceiveNumber();
  //delay(10);
  digitalWrite(LED_BUILTIN,LOW);
  //delay(10);
  //df = 1;
  servo1.write(degree);
} 
