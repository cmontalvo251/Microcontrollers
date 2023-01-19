char val; //Data received from the serial port
#define THROTTLE_HIGH 1523
#define THROTTLE_LOW 1100

int throttle_out = THROTTLE_LOW;

#include <Servo.h>

Servo esc[4];

void setup() {
  Serial.begin(9600);
  for (int idx = 0;idx<4;idx++) {
    esc[idx].attach(8+idx);
  }
}

void loop() {
  if (Serial.available() > 0) {
    val = Serial.read();
    
    if (val == '0') {
      throttle_out = THROTTLE_LOW;
    }
    if (val == '1') {
      throttle_out = THROTTLE_HIGH;
    }
  }

  for (int idx = 0;idx<4;idx++){
    esc[idx].writeMicroseconds(throttle_out);
  }
  
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.println(throttle_out);
}
