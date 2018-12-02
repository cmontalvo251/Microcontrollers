#include <Servo.h>

Servo myservo;

int value = 1100;

char val[15]; //Data received from the serial port

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(7);
}

void loop() {
  // put your main code here, to run repeatedly:
  myservo.writeMicroseconds(value);
  //int value = pulseIn(7,HIGH);
  Serial.println(value);


  if (Serial.available() > 0) {
    for (int idx = 0;idx<5;idx++){
      val[idx] = '0';
    }
    val[0] = '9';
    int length_of_str = Serial.available();
    Serial.println(length_of_str);
    //delay(1000);
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
      ctr++;
      delay(10);
    }
    //delay(100);
    //This means we're changing the nominal throttle
    char throttle_str[4];
    for (int idx = 0;idx<4;idx++) {
      throttle_str[idx] = val[idx];
    }
    Serial.println(throttle_str);
    //delay(1000);
    value = atoi(throttle_str);
  }
}
