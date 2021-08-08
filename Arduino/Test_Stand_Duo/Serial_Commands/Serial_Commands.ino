char val[5]; //Data received from the serial port
char throttle_str[4]; //4 digit throttle command sent to motors in string
#define THROTTLE_MAX 1300
#define THROTTLE_LOW 1100

int throttle_out = THROTTLE_LOW;

#include <Servo.h>

Servo esc[2];
int motors[2];

void setup() {
  Serial.begin(9600);
  for (int idx = 0;idx<2;idx++) {
    motors[idx] = 0;
    //Only need 2 motors for this one.
    esc[idx].attach(11-idx);
  }
}

void loop() {
  if (Serial.available() > 0) {
    for (int idx = 0;idx<5;idx++){
      val[idx] = '0';
    }
    val[0] = '1';
    int length_of_str = Serial.available();
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
      ctr++;
      delay(10);
    }

    //If val starts with a zero it means that we are changing the motors
    if (val[0] == '0') {
      Serial.println("Changing Motors");
      if (length_of_str == 1) {
        //if we just send the zero command shut off all motors
        throttle_out = THROTTLE_LOW;
        Serial.println("SHUTTING DOWN!!!");
      } else {
        //Copy the contents of val into motors
        for (int idx = 1;idx<5;idx++) {
          motors[idx-1] = int(val[idx])-48;
        }
      }
    } else {
      //Interpret as a 4 digit throttle command
      for (int idx = 0;idx<4;idx++){
        throttle_str[idx] = val[idx];
      }
      throttle_out = atoi(throttle_str);
    }
    
  }

  if (throttle_out > THROTTLE_MAX) {
    throttle_out = THROTTLE_MAX;
  }
  if (throttle_out < THROTTLE_LOW) {
    throttle_out = THROTTLE_LOW;
  }
  
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  for (int idx = 0;idx<2;idx++) {
    Serial.print(motors[idx]);
    if (motors[idx] == 1) {
      esc[idx].writeMicroseconds(throttle_out);
    }
    else {
      esc[idx].writeMicroseconds(THROTTLE_LOW);
    }
  }
  Serial.print(" ");
  Serial.println(throttle_out);
}
