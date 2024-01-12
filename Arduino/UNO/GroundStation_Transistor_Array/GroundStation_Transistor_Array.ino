int servoCommand = 90;

#include <Servo.h>

#define AUTOSWITCH 12
#define RESISTORPINA 5
#define RESISTORPINB 6
#define SERVOA 7
#define SERVOB 8
#define SERVOC 9
#define SERVOD 10

Servo servoa, servob, servoc, servod;

void setup() {
  Serial.begin(115200);

  //Attach Servo
  servoa.attach(SERVOA);
  servob.attach(SERVOB);
  servoc.attach(SERVOC);
  servod.attach(SERVOD);

  //Setup RESISTORPIN as output
  pinMode(RESISTORPINA,OUTPUT);
  pinMode(RESISTORPINB,OUTPUT);

  //Setup AUTOSWITCH as input
  pinMode(AUTOSWITCH,INPUT);
}

void loop() {
  // Need to read signal coming from trainer switch on Receiver
  int channelIn = pulseIn(AUTOSWITCH,HIGH);

  int autopilot = 0;
  if (channelIn < 1800) {
    autopilot = 1;
  }
  if (channelIn < 1300) {
    autopilot = 2;
  }

  Serial.print(channelIn);
  Serial.print(" ");
  Serial.print(autopilot);
  Serial.print(" ");
  Serial.println(servoCommand);

  // Default is to send a LOW signal to the Digital Write HIGH or LOW flag
  // If TRAINER SWITCH is HIGH we send HIGH to RESISTORS if AUTOSWITCH is LOW we send a LOW signal to RESISTORS
  if (autopilot) {
    digitalWrite(RESISTORPINA,HIGH);
    digitalWrite(RESISTORPINB,HIGH);
  }
  else {
    digitalWrite(RESISTORPINA,LOW);
    digitalWrite(RESISTORPINB,LOW);
  }

  // If AUTOSWITCH is HIGH it means Autopilot is on 
  // Thus we generate a command for the servo
  // Send a Servo command
  delay(100);
  if (autopilot) {
    servoa.write(servoCommand);
    servob.write(servoCommand);
    servoc.write(servoCommand);
    servod.write(servoCommand);
  }
  if (autopilot == 1) {
    servoCommand+=10;
    if (servoCommand > 120) {
      servoCommand = 60;
    }
  }
  if (autopilot == 2) {
    servoCommand-=10;
    if (servoCommand < 60) {
      servoCommand = 120;
    }
  }
   

}
