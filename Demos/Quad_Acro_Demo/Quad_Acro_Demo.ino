//Declaring Variables
byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;
int esc_commands[4]; //0 = top left, 1 = bottom left, 2 = top right, 3 = bottom right
boolean QUADARMED;
int MODE = 0;

//Set rate of receiver_input_channel_roll/pitch and yaw
#define RATE  0.2 //DO NOT SET RATE > 1!!!!!
#define ANGLE_MAX 30 //Maximum rotation in degrees
#define kp  0.8 //0.8  //This assumes roll, pitch yaw are in degrees - These units have been verified 9/26/2017
#define kd  0.3 //0.3   //This assume r,p,y rate are in deg/ses - These units have been verified 9/26/2017
#define PRINTSERIAL true //set to false when doing flight tests but true when doing classroom demos
#define THROTTLE_HOVER 1200 //Need to tune this. Not sure what throttle hover is. Need to do some experiments
#define THROTTLE_MIN 1100 //Props will spin but craft will not take off
#define THROTTLE_OFF 1100 //Props won't spin at this RPM
#define THROTTLE_MAX 2100 //
#define THROTTLE_SLOPE 1.0 //DO NOT SET OVER 0.5 YOU WILL HAVE INSANELY TWITCHY THROTTLE RESPONSE

float rate_yaw = RATE/2.0;
float roll=0,pitch=0,yaw=0,roll_rate=0,pitch_rate=0,yaw_rate=0;
float pitch_command,roll_command;

//Setup servos (ESCs)
#include <Servo.h>
Servo TOP_LEFT;
Servo BOTTOM_LEFT;
Servo TOP_RIGHT;
Servo BOTTOM_RIGHT;

//Setup routine
void setup(){

  if (PRINTSERIAL) {
    Serial.begin(9600);
  }
  
  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  PCMSK2 |= (1 << PCINT19); //Pin 3 - Roll
  PCMSK2 |= (1 << PCINT20); //Pin 4 - Pitch
  PCMSK2 |= (1 << PCINT21); //Pin 5 - Yaw
  PCMSK2 |= (1 << PCINT22); //Pin 6 - Gear
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Attach ESCs
  TOP_LEFT.attach(8); //this is ESC4 using CleanFlight, This plugs into pin closest to camera
  BOTTOM_LEFT.attach(9); //this is ESC3 using CleanFlight
  TOP_RIGHT.attach(10); //this is ESC2 using CleanFlight
  BOTTOM_RIGHT.attach(11); //this is ESC1 using CleanFlight, This plugs into pin closest to receiver
  //BTW Ground is towards the right of the quad and PWR_In is towards the left.

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 0;
  receiver_input_channel_yaw = 0;
  receiver_input_channel_roll = 0;
  receiver_input_channel_pitch = 0;
  receiver_input_channel_gear = 0;
  QUADARMED = 0;
}

//Main program loop
void loop(){

  //Make sure throttle is down before you arm
  if ((receiver_input_channel_throttle < 1150) and (receiver_input_channel_throttle > 990) and (receiver_input_channel_gear > 1000) and (receiver_input_channel_gear < 1500)) {
    QUADARMED = true;
  }

  //If you switch off Gear make sure to disarm rotors
  if ((QUADARMED) and (receiver_input_channel_gear > 1800)) {
    QUADARMED = false;
  }

  //Compute throttle no matter what
  //For now just compute throttle based on receiver_input_channel
  //Eventually I want to have throttle controlled by the pressure sensor
  //If possible
  //Compute Throttle based on receiver_input_channel
  int throttle_out = throttle_curve(receiver_input_channel_throttle)+THROTTLE_MIN - THROTTLE_OFF;

  //Based on the Gear Value you need to compute Acro mode or Attitude/Altitude Hold Mode
  //Gear switch = 1900,1500,1100
  if ((receiver_input_channel_gear < 1500) and (receiver_input_channel_gear > 1000) and QUADARMED) {
    //ACRO MODE
    Serial.print("A ");
    acro_mode(throttle_out); 
    MODE = 1;
  } else {
    //If Gear is giving something wonky just default to 1100
    for (int idx = 0;idx<4;idx++) {
      esc_commands[idx] = THROTTLE_OFF;
    }
    MODE = 0;
  }

  //For minimum saturation we want to do something
  int SATURATION_LOW = THROTTLE_MIN;
  if (throttle_out > THROTTLE_HOVER) {
    SATURATION_LOW = THROTTLE_HOVER;
  } else {
    SATURATION_LOW = THROTTLE_MIN;
  }

  //Make sure to add the saturation blocks
  for (int idx = 0;idx<4;idx++) {
    if (esc_commands[idx] > THROTTLE_MAX) {
      esc_commands[idx] = THROTTLE_MAX;
    }
    if (MODE != 0) {
      if (esc_commands[idx] < SATURATION_LOW) {
        esc_commands[idx] = SATURATION_LOW;
      }
    }
  }

  //Send Command to rotors
  TOP_LEFT.writeMicroseconds(esc_commands[0]);
  BOTTOM_LEFT.writeMicroseconds(esc_commands[1]);
  TOP_RIGHT.writeMicroseconds(esc_commands[2]);
  BOTTOM_RIGHT.writeMicroseconds(esc_commands[3]);
    

  //Print Controller Health
  if (PRINTSERIAL) {
    Serial.print("T = ");
    Serial.print(receiver_input_channel_throttle);
    Serial.print(" ");

    Serial.print("Y = ");
    Serial.print(receiver_input_channel_yaw);
    Serial.print(" ");
    
    //Only Output Roll and Pitch Signals if Mode is less than 2
    if (MODE < 2) {
      Serial.print("P = ");
      Serial.print(receiver_input_channel_pitch);
      Serial.print(" ");
  
      Serial.print("R = ");
      Serial.print(receiver_input_channel_roll);
      Serial.print(" ");

      Serial.print("G = ");
      Serial.print(receiver_input_channel_gear);
      Serial.print(" ");
    } else {
      Serial.print("P_C = ");
      Serial.print(pitch_command);
      Serial.print(" ");

      Serial.print("R_C = ");
      Serial.print(roll_command);
      Serial.print(" ");
    }

  //If you're in attitude hold mode print the Roll Pitch and Yaw Angles and Rates
  if (MODE == 2) {
    Serial.print("RPY = ");
    Serial.print(roll);
    Serial.print(" ");
    
    Serial.print(pitch);
    Serial.print(" ");

    Serial.print(yaw);
    Serial.print(" ");

    Serial.print(roll_rate);
    Serial.print(" ");

    Serial.print(pitch_rate);
    Serial.print(" ");

    Serial.print(yaw_rate);
    Serial.print(" ");
  }

  Serial.print("TL = ");
  Serial.print(esc_commands[0]);
  Serial.print(" ");

  Serial.print("BL = ");
  Serial.print(esc_commands[1]);
  Serial.print(" ");

  Serial.print("TR = ");
  Serial.print(esc_commands[2]);
  Serial.print(" ");

  Serial.print("BR = ");
  Serial.print(esc_commands[3]);
  Serial.print(" ");

  Serial.print("\n");
  }
  
}

//Compute Fancy Throttle
int throttle_curve(int tin) {
  float dt = float(tin)-THROTTLE_MIN;
  //float tout = p1 + p2*dt + p3*dt^2 + p4*dt^3 + p5*dt^4 + p6*dt^5;
  //for now just do linear
  float tout = THROTTLE_MIN + THROTTLE_SLOPE*dt;
  return int(tout);
}

//Acro Mode Computation
void acro_mode(int throttle_out) {

  for (int idx = 0;idx<4;idx++) {
    esc_commands[idx] = throttle_out;
  }
}

ISR(PCINT2_vect) { //this is for pins 0-7
  current_time = micros();
  if (PIND & B00000100) {                                      //Is input 2 high?
    if(last_channel_throttle == 0){                                   //Input changed from 0 to 1
      last_channel_throttle = 1;                                      //Remember current input state
      timer_throttle = current_time;                                  //Set timer_throttle to current_time
    }
  }
  else if(last_channel_throttle == 1){                                //Input is not high and changed from 1 to 0
    last_channel_throttle = 0;                                        //Remember current input state
    receiver_input_channel_throttle = current_time - timer_throttle;         //Channel throttle is current_time - timer_throttle 
    //which is essentially the length of the pulse
  }

  if (PIND & B00001000) {
    if (last_channel_roll == 0) {
      last_channel_roll = 1;
      timer_roll = current_time;
    }
  }
  else if (last_channel_roll == 1) {
    last_channel_roll = 0;
    receiver_input_channel_roll = current_time - timer_roll;
  }

  if (PIND & B00010000) {
    if (last_channel_pitch == 0) {
      last_channel_pitch = 1;
      timer_pitch = current_time;
    }
  }
  else if (last_channel_pitch == 1) {
    last_channel_pitch = 0;
    receiver_input_channel_pitch = current_time - timer_pitch;
  }

  if (PIND & B00100000) {
    if (last_channel_yaw == 0) {
      last_channel_yaw = 1;
      timer_yaw = current_time;
    }
  }
  else if (last_channel_yaw == 1) {
    last_channel_yaw = 0;
    receiver_input_channel_yaw = current_time - timer_yaw;
  }

  if (PIND & B01000000) {
    if (last_channel_gear == 0) {
      last_channel_gear = 1;
      timer_gear = current_time;
    }
  }
  else if (last_channel_gear == 1) {
    last_channel_gear = 0;
    receiver_input_channel_gear = current_time - timer_gear;
  }
  
}

