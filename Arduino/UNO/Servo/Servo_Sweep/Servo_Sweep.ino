
#include <Servo.h>
Servo servo1;

void setup() {
//Serial.begin(9600);
servo1.attach(9);

}

void loop() {

  
servo1.write(40);
delay (3000);

servo1.write(140);
delay(3000);

}
