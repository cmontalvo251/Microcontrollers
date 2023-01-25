#ifndef COMMANDS_H
#define COMMANDS_H

#include "receiver.h"
#include "i2c.h"
#include <Servo.h>

class commands {
  public:
    //Setup servos (ESCs)
    Servo THROTTLEServo,AILERONServo,RUDDERServo,ELEVATORServo,GEAR0Servo,GEAR1Servo;
    float lastSendCommandsTime = 0;
    void setup_servo();
    void send_Commands(float);
    void print_Commands();
    void compute_Control(receiver,i2c);
    void print_SD_Commands(File);
    float lastComputeControlTime = 0;
    float additional_throttle = 0;
    int THROTTLE_CMD=1000,AILERON_CMD=1000,RUDDER_CMD=1000,ELEVATOR_CMD=1000,GEAR0_CMD=1000,GEAR1_CMD=1000;
    commands(); //Constructor
};

#endif COMMANDS_H
