//Don't plug in Elegoo into PC and battery at the same time

#include <Servo.h>

Servo myservo; //Set up servo
Servo mymotor; //Set up motor

unsigned long servo_pos, motor_speed, auto_switch; //Servo position, motor speed, and autonomous switch in ms

void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT); //Reads servo commands on pin 7, Servo uses pin 2 on receiver
  pinMode(5, INPUT); //Reads motor commands on pin 5, motor uses pin 1 on receiver and F switch must be on 0
  pinMode(3, INPUT); //Reads switch A on transmitter with pin 3 on Elegoo, uses pin 5 on receiver
  myservo.attach(9); //Sends servo commands to pin 9
  mymotor.attach(11); //Sends motor commands to pin 11
  Serial.println("Ready");
}

void loop() {
  servo_pos = pulseIn(7, HIGH);
  motor_speed = pulseIn(5, HIGH);
  auto_switch = pulseIn(3, HIGH);
  //Serial.print(motor_speed);
  //Serial.print(" ");
  Serial.print(servo_pos);
  Serial.print('\n');
  //Serial.println(auto_switch);
  //switch A being on 1 is manual, and 0 is autopilot
  if (1800 >= auto_switch){
    if (servo_pos!=0){
      myservo.writeMicroseconds(servo_pos);
    }

    else {
      myservo.write(90); //sets servo to 90 degrees if no signal
    }

    if (motor_speed!=0){
      mymotor.writeMicroseconds(motor_speed);
    }

    else {
      mymotor.write(0); //stops motor if no signal
    }
    delay(15);
  }
  
  if (auto_switch >= 1800){ //Checks if A switch flipped to autopilot
    for (servo_pos = 90; servo_pos <= 135; servo_pos += 1){ //Turns left
      auto_switch = pulseIn(3, HIGH);
      //Serial.println(auto_switch);
      if (auto_switch <= 1800){break;} //Checks if A switch flipped back to RC
      myservo.write(servo_pos);
      mymotor.writeMicroseconds(1700); //Needs fully charged battery for low speeds
      delay(15);
    }
    for (servo_pos = 135; servo_pos >= 45; servo_pos -= 1){ //Turns right
      auto_switch = pulseIn(3, HIGH);
      //Serial.println(auto_switch);
      if (auto_switch <= 1800){break;} //Checks if A switch flipped back to RC
      myservo.write(servo_pos);
      mymotor.writeMicroseconds(1700);
      delay(15);
    }
    for (servo_pos = 45; servo_pos <=90; servo_pos += 1){ //Goes back to center
      auto_switch = pulseIn(3, HIGH);
      //Serial.println(auto_switch);
      if (auto_switch <= 1800){break;} //Checks if A switch flipped back to RC
      myservo.write(servo_pos);
      mymotor.writeMicroseconds(1700);
      delay(15);
    }
  }


}