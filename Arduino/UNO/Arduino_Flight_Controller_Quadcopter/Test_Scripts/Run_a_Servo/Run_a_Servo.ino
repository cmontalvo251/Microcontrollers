#include <Servo.h>

Servo myservo;

float angle = 0;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  angle += 10;
  if (angle > 180) {
    angle = 0;
  }
  myservo.write(angle);
  delay(500);
}
