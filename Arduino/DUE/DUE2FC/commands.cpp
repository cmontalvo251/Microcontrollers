#include "userparams.h"
#include "commands.h"
#include "mathHelp.h"
#include "i2c.h"
#include "sd.h"

commands::commands() {
  
}

void commands::print_Commands() {
  Serial.print(" T=");
  Serial.print(THROTTLE_CMD);
  Serial.print(" ");

  Serial.print("Y=");
  Serial.print(RUDDER_CMD);
  Serial.print(" ");
    
  Serial.print("P=");
  Serial.print(ELEVATOR_CMD);
  Serial.print(" ");
  
  Serial.print("R=");
  Serial.print(AILERON_CMD);
  Serial.print(" ");
  
  Serial.print("G0,1=");
  Serial.print(GEAR0_CMD);
  Serial.print(",");
  Serial.print(GEAR1_CMD);
  Serial.print(" ");
}

void commands::print_SD_Commands(File myFile) {
  myFile.print(THROTTLE_CMD);
  myFile.print(" ");
  myFile.print(RUDDER_CMD);
  myFile.print(" ");
  myFile.print(ELEVATOR_CMD);
  myFile.print(" ");
  myFile.print(AILERON_CMD);
  myFile.print(" ");
  //myFile.print(GEAR0_CMD);
  //myFile.print(" ");
  //myFile.print(GEAR1_CMD);
  //myFile.print(" ");
  myFile.print(additional_throttle);
  myFile.print(" ");
  //myFile.print(delP);
  //myFile.print(" ");
  //myFile.print(delZ);
  //myFile.print(" ");
  //myFile.print(alt_int);
  //myFile.print(" ");
  //myFile.print(Az);
}

void commands::compute_Control(receiver RX,i2c myi2c) {
  #ifdef DEBUGPRINTS
  Serial.print("Computing Control. Time = ");
  //Serial.print(loopTimer);
  //Serial.print(" ");
  //Serial.print("Elapsed Time = ");
  //Serial.println(loopTimer - lastComputeControlTime,6);
  Serial.println(lastComputeControlTime);
  Serial.print(" MODE = ");
  Serial.print(RX.MODE);
  Serial.println();
  #endif
  lastComputeControlTime = millis()/1000.0;
  switch (RX.MODE) {
  case 0:
    //No control law so just send all standard commands
    //Actually. In this mode the quad needs to remain in its off position
    //so we actually just send 1200 1200
    THROTTLE_CMD = RX.receiver_input_channel_throttle;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;
    GEAR0_CMD = 1200;
    GEAR1_CMD = 1200;
    #ifdef DEBUGPRINTS
    Serial.println("MODE 0 Computed");
    //THROTTLE_CMD = 1234;
    print_Commands();
    RX.print_RX();
    Serial.println();
    #endif
    break;
  case 1:
    //Need to Arm Quad but I have inputs coming in and then I need to tell the Naze what to do so 
    //I need GEAR0 to be arm on off since that's the standard 
    GEAR0_CMD = 1800; //Gear0 will arm the quad
    //Then I need GEAR1 to to be either RATE MODE or STAB MODE so it's really just on or off in this case it's off or RATE MODE
    GEAR1_CMD = 1200; //Gear1 will pick between rate and stab
    //Everything else is input output
    THROTTLE_CMD = RX.receiver_input_channel_throttle;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;
    break;
  case 2:
    //Same thing as above except GEAR1 will be HIGH
    GEAR0_CMD = 1800;
    GEAR1_CMD = 1800;
    THROTTLE_CMD = RX.receiver_input_channel_throttle;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;
    break;
  case 3:
    //For the next three the GEARS will both be HIGH but the CMD will now be autopilots
    GEAR0_CMD = 1800;
    GEAR1_CMD = 1800;
    //Test Number 1 - Send constant throttle command
    THROTTLE_CMD = THR_HOVER;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;

    //Test Number 2 - Send 20 degree bank angle command
    //float PHI_CMD = 20.0;
    //float m = 2.0*PHI_MAX/(PWM_MAX - PWM_MIN);
    //AILERON_CMD = PHI_CMD/m + PWM_MID;
    //THROTTLE_CMD = receiver_input_channel_throttle;
    //ELEVATOR_CMD = receiver_input_channel_pitch;
    //RUDDER_CMD = receiver_input_channel_yaw;

    //Test Number 3 - Try and get the Quad to maintain an equilibrium altitude
    myi2c.delZ = (myi2c.altitude_command - myi2c.altitude);
    myi2c.alt_int+=myi2c.delZ; //As I climb this number will get bigger but it will accumulate over time
    additional_throttle = KP_ALTITUDE*myi2c.delZ + KI_ALTITUDE*myi2c.alt_int;
    additional_throttle = CONSTRAIN(additional_throttle,-250.0,250.0);
    THROTTLE_CMD += additional_throttle;
    //AILERON_CMD = receiver_input_channel_roll;
    //ELEVATOR_CMD = receiver_input_channel_pitch;
    //RUDDER_CMD = receiver_input_channel_yaw;
    break;
  case 4:
    //Since we don't have these coded yet I will just make these input output for now
    GEAR0_CMD = 1800;
    GEAR1_CMD = 1800;
    THROTTLE_CMD = RX.receiver_input_channel_throttle;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;
    break;
  case 5:
    GEAR0_CMD = 1800;
    GEAR1_CMD = 1800;
    THROTTLE_CMD = RX.receiver_input_channel_throttle;
    AILERON_CMD = RX.receiver_input_channel_roll;
    ELEVATOR_CMD = RX.receiver_input_channel_pitch;
    RUDDER_CMD = RX.receiver_input_channel_yaw;
    break;
  }
}

void commands::setup_servo() {
  //Change Resolution to 12? No. Looks like this only works for DUE
  //analogWriteResolution(12); 
  
  //Attach COMMANDS to Send to NAZE - Comment out these for now for initial testing
  THROTTLEServo.attach(13);  //12
  AILERONServo.attach(12); 
  ELEVATORServo.attach(11); 
  RUDDERServo.attach(10); 
  GEAR1Servo.attach(9);
  GEAR0Servo.attach(8);
}

void commands::send_Commands(float loopTimer) {
  #ifdef DEBUGPRINTS
  Serial.print("Sending Command to Motors. Time = ");
  Serial.print(loopTimer);
  Serial.print(" Elapsed Time = ");
  Serial.println(loopTimer-lastSendCommandsTime,6);
  #endif
  lastSendCommandsTime = millis()/1000;
  THROTTLEServo.writeMicroseconds(THROTTLE_CMD);
  AILERONServo.writeMicroseconds(AILERON_CMD);
  ELEVATORServo.writeMicroseconds(ELEVATOR_CMD);
  RUDDERServo.writeMicroseconds(RUDDER_CMD);
  GEAR0Servo.writeMicroseconds(GEAR0_CMD);
  GEAR1Servo.writeMicroseconds(GEAR1_CMD);
}
