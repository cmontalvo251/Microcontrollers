//PRELIMINARY EXPERIMENTS
//Rev 0.0 - Blinky Light Test - June 2017
//Rev 0.1 - Read_Receiver Signal - June 2017
//Rev 0.2 - Read_Receiver Using Interrupts - June 2017
//Rev 0.3 - Rotate a Servo - June 2017
//Rev 0.4 - Control a Servo with Receiver Signals - June 2017
//Rev 0.5 - Read 5 Channels from Spektrum Receiver - August 2017
//Rev 0.6 - Program Acro Mode, Arm Switch and Saturation Blocks - August 2017

//BREADBOARDING OF CIRCUIT
//Rev 1.0 - Send Control Commands to Motors - August 2017
//Rev 1.1 - Add in Attitude Hold Logic - September 2017
//Rev 1.2 - Added in Sensor feedback from 9DOF sensor - September 2017
//Rev 1.3 - Created PID controller for Roll and Pitch Channels - September 2017

//AUTOPILOT LOGIC AND GROUND TESTS
//Rev 2.0 - Ground Tested Acro and Hold Mode with no props - September 2017
//Rev 2.1 - Soldered PCB and checked voltages. Receiver signals are working correctly. Roll and Yaw wires were crossed - 9/25/2017
//Rev 2.2 - Set RPY to euler.(-z,y,x) and pqr to rate_fc.(x,-y,-z) - 9/26/2017
//Rev 2.3 - ACRO Mode gains tuned on ground - 9/27/2017
//Rev 2.4 - HOLD Mode gains tuned on ground. Read y for Prop Test - 9/27/2017

//FLIGHT TESTS OF AFC
//Rev 3.0 - Perform Ground Test with Rotors and verify everything checks out - 9/29/2017
//Rev 3.1 - Tuned some minimum throttle settings. I believe I was having a situation where I was in hover and a slight 
//move of the stick would put half the rotors close to THROTTLE_MIN which would just put half the quad on the ground. - 9/30/2017
//Rev 3.2 - Modeled the transfer function and tuned some gains. Fixed some more THROTTLE_MIN stuff which I believe works
//better
//Rev 3.3 - Made sure signs on Kp and kd were good. added a small filter on roll pitch and yaw due to some pretty bad jitter.
//Also fixed some horrible unit problems.
//Rev 3.4 - Changed PRINTSERIAL to a #if so code could run faster in release mode. Added a constrain command as well. Also added a scheduler
//for the euler angle routines.

//Future Work

//--Perform Acro in Flight test -- Tune rates based on flight - Flight test failed in class on 10/24/2017
//--Perform Attitude Hold Mode in flight -- tune gains - Using tuned gains from 10/24/2017 the craft still failed. Need to check sign on pitch gains
//--Add in Yaw control for Attitude hold
//--Change yaw input to yaw rate control and integrate to get yaw command
//--Test Yaw controller in flight
//--Add in pressure sensor plus filter
//--Convert pressure to climb rate
//--Change throttle setting to climb rate command
//--Test throttle controller in flight

//Declaring Variables
byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
int receiver_input_channel_throttle_new, receiver_input_channel_yaw_new, receiver_input_channel_pitch_new, receiver_input_channel_roll_new, receiver_input_channel_gear_new;
int receiver_input_channel_throttle_old, receiver_input_channel_yaw_old, receiver_input_channel_pitch_old, receiver_input_channel_roll_old, receiver_input_channel_gear_old;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;
int esc_commands[4]; //0 = top left, 1 = bottom left, 2 = top right, 3 = bottom right
boolean QUADARMED;
int MODE = 0;
float int_pitch=0,int_roll=0;

//////////////USER PARAMETERS////////////////

//Set rate of receiver_input_channel_roll/pitch and yaw
//Lowered it to 0.04 after I set PRINTSERIAL to false.
//Ok so full acro mode is actually impossible. With all 4 rotors on with the same command the quad still oscillates
//You need at least a small controller to make this thing go. Set RATE back to 0.08
#define ANGLE_MAX 30 //Maximum rotation in degrees
//Gonna have to start all over again with new gains and change the NEXT_TIME variable.
#define RATE_MAX 30 //30 Deg/sec
#define kp_rate 2.0 //For rate mode

#define PRINTSERIAL false //set to false when doing flight tests but true when doing classroom demos
#define THROTTLE_HOVER 1150 //
#define THROTTLE_MIN 1170 //Props will spin but craft will not take off
#define THROTTLE_OFF 1100 //Props won't spin at this RPM
#define THROTTLE_MAX 1700 //Max props just because
#define THROTTLE_SLOPE 0.5 //DO NOT SET OVER 0.5 YOU WILL HAVE INSANELY TWITCHY THROTTLE RESPONSE
#define REC_MIN 1100
#define REC_MAX 1900

////////////////////////////////////////////////////////////////


//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//Pressure and Temperature Sensor
#include <Wire.h>
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;

//Setup servos (ESCs)
#include <Servo.h>
Servo TOP_LEFT;
Servo BOTTOM_LEFT;
Servo TOP_RIGHT;
Servo BOTTOM_RIGHT;

float roll=0,pitch=0,yaw=0,roll_rate=0,pitch_rate=0,yaw_rate=0;
float pitch_command,roll_command;
imu::Vector<3> euler;
imu::Vector<3> rate_fc;
imu::Vector<3> euler_prev;
imu::Vector<3> rate_fc_prev;
float getEulerTime = 0;

//Setup routine
void setup(){

  #if PRINTSERIAL
    Serial.begin(9600);
  #endif

  //Start the Pressure Sensor
  mpl115a2.begin();

  #if PRINTSERIAL
  Serial.println("Pressure Sensor Initialized");
  #endif

  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    #if PRINTSERIAL
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    #endif
    while(1);
  }

  #if PRINTSERIAL
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
  #endif
  
  delay(1000);
    
  bno.setExtCrystalUse(true);

  //Change Resolution to 12? No. Looks like this only works for DUE
  //analogWriteResolution(12); 

  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  PCMSK2 |= (1 << PCINT19); //Pin 3 - Roll
  PCMSK2 |= (1 << PCINT20); //Pin 4 - Pitch
  PCMSK2 |= (1 << PCINT21); //Pin 5 - Yaw
  PCMSK2 |= (1 << PCINT22); //Pin 6 - Gear
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Attach ESCs
  //Had I known this before I would have used different pins for the MOTORS and RECEIVERS
  //Actually I'm not sure that would have worked. PCIE2 is 0-7 and PCIE0 is Pins 8-13
  //So PCIE2 is (3,5,6) only 3 and PCIE0 is (9,10,11) again only 3. So I have to use the writeMicroseconds function
  //So let's hope I can get my quad to fly just using writeMicroSeconds
  //PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite() function.
  TOP_LEFT.attach(8); //this is ESC4 using CleanFlight, This plugs into pin closest to camera
  BOTTOM_LEFT.attach(9); //this is ESC3 using CleanFlight
  TOP_RIGHT.attach(10); //this is ESC2 using CleanFlight
  BOTTOM_RIGHT.attach(11); //this is ESC1 using CleanFlight, This plugs into pin closest to receiver
  //BTW Ground is towards the right of the quad and PWR_In is towards the left when looking from the back

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 0;
  receiver_input_channel_yaw = 0;
  receiver_input_channel_roll = 0;
  receiver_input_channel_pitch = 0;
  receiver_input_channel_gear = 0;
  QUADARMED = 0;

  //Initialize Filtered States
  euler_prev.x() = 0;
  euler_prev.y() = 0;
  euler_prev.z() = 0;
  rate_fc_prev.x() = 0;
  rate_fc_prev.y() = 0;
  rate_fc_prev.z() = 0;
  
}

//Main program loop
void loop(){

  //Make sure throttle is down before you arm
  if ((receiver_input_channel_throttle < 1120) and (receiver_input_channel_throttle > 990) and (receiver_input_channel_gear > 1400) and (receiver_input_channel_gear < 1900)) {
    QUADARMED = true;
  }

  //If you switch off Gear make sure to disarm rotors
  if ((QUADARMED) and (receiver_input_channel_gear > 1800)) {
    QUADARMED = false;
    int_pitch = 0;
    int_roll = 0;
  }

  //Compute throttle no matter what
  //For now just compute throttle based on receiver_input_channel
  //Eventually I want to have throttle controlled by the pressure sensor
  //If possible
  //Compute Throttle based on receiver_input_channel
  int throttle_out = throttle_curve(receiver_input_channel_throttle);

  //Based on the Gear Value you need to compute Acro mode or Attitude/Altitude Hold Mode
  //Gear switch = 1900,1500,1100
  if ((receiver_input_channel_gear < 1600) and (receiver_input_channel_gear > 1400) and QUADARMED) {
    //MANUAL MODE
    #if PRINTSERIAL
    Serial.print("M ");
    #endif
    manual(throttle_out); 
    MODE = 1;
    int_pitch = 0;
    int_roll = 0;
  }
  else if ((receiver_input_channel_gear < 1200) and (receiver_input_channel_gear > 1000) and QUADARMED) {
    //Attitude Hold
    #if PRINTSERIAL
    Serial.print("A ");
    #endif
    hold_mode(throttle_out);
    MODE = 2;
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
  #if PRINTSERIAL
    Serial.print("T = ");
    Serial.print(throttle_out);
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
      //Serial.print("P_C = ");
      //Serial.print(pitch_command);
      //Serial.print(" ");

      //Serial.print("R_C = ");
      //Serial.print(roll_command);
      //Serial.print(" ");
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
  #endif
  
}

//Compute Fancy Throttle
int throttle_curve(int tin) {
  float dt = float(tin)-THROTTLE_MIN;
  //float tout = p1 + p2*dt + p3*dt^2 + p4*dt^3 + p5*dt^4 + p6*dt^5;
  //for now just do linear
  float tout = THROTTLE_MIN + THROTTLE_SLOPE*dt;
  return int(tout);
}

//FULL MANUAL
void manual(int throttle_out) {
  for (int idx = 0;idx<4;idx++) {
    esc_commands[idx] = throttle_out;
  }

  float RATE = 0.08;
  float rate_yaw = 0.04;

  //Generate Commands to ESCs
  //Top Left
  esc_commands[0] += RATE*(receiver_input_channel_roll-1500) - RATE*(receiver_input_channel_pitch-1500) - rate_yaw*(receiver_input_channel_yaw-1500);
  //Bottom Left
  esc_commands[1] += RATE*(receiver_input_channel_roll-1500) + RATE*(receiver_input_channel_pitch-1500) + rate_yaw*(receiver_input_channel_yaw-1500);
  //Top Right
  esc_commands[2] += -RATE*(receiver_input_channel_roll-1500) - RATE*(receiver_input_channel_pitch-1500) + rate_yaw*(receiver_input_channel_yaw-1500);
  //Bottom Right
  esc_commands[3] += -RATE*(receiver_input_channel_roll-1500) + RATE*(receiver_input_channel_pitch-1500) - rate_yaw*(receiver_input_channel_yaw-1500);
}

//Acro Mode Computation - Rate Command
void acro_mode(int throttle_out) {

  float NEXT_TIME = 1.0/10.0;

  //Query the Roll Pitch and Yaw Rates - Every loop? Let's see if this works.
  //I think these are being queried too fast Let's slow it down to like 100 Hz? 
  if ((millis()/1000.0-getEulerTime) > NEXT_TIME) {
    rate_fc = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    getEulerTime = millis()/1000.0 + NEXT_TIME;
  }

  float FILTERVALUE = 0;
  
  //Filter Signal
  if (FILTERVALUE > 0) {
    rate_fc.x() = rate_fc.x()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.x();
    rate_fc.y() = rate_fc.y()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.y();
    rate_fc.z() = rate_fc.z()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.z();
    rate_fc_prev.x() = rate_fc.x();
    rate_fc_prev.y() = rate_fc.y();
    rate_fc_prev.z() = rate_fc.z();
  }
  
  //Need to verify this is right simply through experimentation
  roll_rate = PI/180.0*rate_fc.x();
  pitch_rate = -PI/180.0*rate_fc.y();
  yaw_rate = -PI/180.0*rate_fc.z();

  //Using receiver_input_channel_roll and pitch convert to +- 30 degrees
  // y = (30)/(400)*(roll_in - 1500) - when roll_in = 1900 you get 1900-1500 = 400*30/400 = 30 degrees
  roll_command = PI/180.0*RATE_MAX/400.0*(receiver_input_channel_roll - 1500);
  pitch_command = -PI/180.0*RATE_MAX/400.0*(receiver_input_channel_pitch - 1500);

  //Further more set the yaw commands equal to acro mode for now.

  //In order to get roll_effort and pitch_effort I need to design a proportional controller such that
  float pitch_effort = kp_rate*(pitch_rate - pitch_command);
  float roll_effort = kp_rate*(roll_rate - roll_command);

  for (int idx = 0;idx<4;idx++) {
    esc_commands[idx] = throttle_out;
  }

  float rate_yaw = 0.04;

  //Generate Commands to ESCs
  //Top Left
  esc_commands[0] += -rate_yaw*(receiver_input_channel_yaw-1500) - roll_effort - pitch_effort;
  //Bottom Left
  esc_commands[1] += rate_yaw*(receiver_input_channel_yaw-1500) - roll_effort + pitch_effort;
  //Top Right
  esc_commands[2] += rate_yaw*(receiver_input_channel_yaw-1500) + roll_effort - pitch_effort;
  //Bottom Right
  esc_commands[3] += -rate_yaw*(receiver_input_channel_yaw-1500) + roll_effort + pitch_effort;
}

//Attitude Hold Mode
void hold_mode(int throttle_out) {

  float NEXT_TIME = 1.0/10.0; //How fast to update the euler angles. 1/100 is 100 Hz or 100 times a second or every 0.01 seconds
  
  //Query the Roll Pitch and Yaw Rates - Every loop? Let's see if this works.
  //I think these are being queried too fast Let's slow it down to like 100 Hz? 
  if ((millis()/1000.0-getEulerTime) > NEXT_TIME) {
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    rate_fc = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    getEulerTime = millis()/1000.0 + NEXT_TIME;
  }
  //Grab pressure data
  //pressureKPA = mpl115a2.getPressure();

  float FILTERVALUE = 0; //PRINTSERIAL = false. Not 100% on this anymore. Turns out calling the Euler angles

  //Filter Signal
  if (FILTERVALUE > 0) {
    euler.x() = euler.x()*(1-FILTERVALUE) + FILTERVALUE*euler_prev.x();
    euler.y() = euler.y()*(1-FILTERVALUE) + FILTERVALUE*euler_prev.y();
    euler.z() = euler.z()*(1-FILTERVALUE) + FILTERVALUE*euler_prev.z();
    rate_fc.x() = rate_fc.x()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.x();
    rate_fc.y() = rate_fc.y()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.y();
    rate_fc.z() = rate_fc.z()*(1-FILTERVALUE) + FILTERVALUE*rate_fc_prev.z();
    euler_prev.x() = euler.x();
    euler_prev.y() = euler.y();
    euler_prev.z() = euler.z();
    rate_fc_prev.x() = rate_fc.x();
    rate_fc_prev.y() = rate_fc.y();
    rate_fc_prev.z() = rate_fc.z();
  }
  
  //Need to verify this is right simply through experimentation
  roll = PI/180.0*(-euler.z()+9.0); //The 9.0 is because the sensor is not mounted flat on the PCB
  pitch = PI/180.0*euler.y();
  yaw = PI/180.0*euler.x();
  roll_rate = PI/180.0*rate_fc.x();
  pitch_rate = -PI/180.0*rate_fc.y();
  yaw_rate = -PI/180.0*rate_fc.z();

  //Using receiver_input_channel_roll and pitch convert to +- 30 degrees
  // y = (30)/(400)*(roll_in - 1500) - when roll_in = 1900 you get 1900-1500 = 400*30/400 = 30 degrees
  roll_command = PI/180.0*ANGLE_MAX/400.0*(receiver_input_channel_roll - 1500);
  pitch_command = -PI/180.0*ANGLE_MAX/400.0*(receiver_input_channel_pitch - 1500);
  //roll_command = 0.0;
  //pitch_command = 0.0;

  //Further more set the yaw commands equal to acro mode for now.

  float kp = 15.0; //  //This assumes roll, pitch yaw are in rad - Check the code to see the conversion
  float kd = 0.5; // This assume r,p,y rate are in rad/sec - Check the code to see the conversion 10/25/2017
  float ki = 8.0; 

  float rate_yaw = 0.1;

  float error_roll = roll - roll_command;
  float error_pitch = pitch - pitch_command;

  float dt = 0.001;

  int_roll += error_roll*dt;
  int_pitch += error_pitch*dt;

  //In order to get roll_effort and pitch_effort I need to design a proportional controller such that
  float pitch_effort = kp*(pitch - pitch_command) + kd*pitch_rate + ki*int_pitch;
  float roll_effort = kp*(roll - roll_command) + kd*roll_rate + ki*int_roll;
  float BIAS_TOP = 0;

  for (int idx = 0;idx<4;idx++) {
    esc_commands[idx] = throttle_out;
  }

  //Debugging Pitch
  //rate_yaw = 0;
  //roll_effort = 0;

  //Generate Commands to ESCs
  //Top Left
  esc_commands[0] += -rate_yaw*(receiver_input_channel_yaw-1500) - roll_effort - pitch_effort;
  //Bottom Left
  esc_commands[1] += rate_yaw*(receiver_input_channel_yaw-1500) - roll_effort + pitch_effort;
  //Top Right
  esc_commands[2] += rate_yaw*(receiver_input_channel_yaw-1500) + roll_effort - pitch_effort;
  //Bottom Right
  esc_commands[3] += -rate_yaw*(receiver_input_channel_yaw-1500) + roll_effort + pitch_effort;

  //Add a BIAS
  esc_commands[0]+=BIAS_TOP;
  esc_commands[2]+=BIAS_TOP;
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
    receiver_input_channel_throttle_new = current_time - timer_throttle;         //Channel throttle is current_time - timer_throttle 
    //which is essentially the length of the pulse
    if ((receiver_input_channel_throttle_new > REC_MAX) || (receiver_input_channel_throttle_new < REC_MIN)) {
      receiver_input_channel_throttle = receiver_input_channel_throttle_old;
    } else {
      receiver_input_channel_throttle = receiver_input_channel_throttle_new;
      receiver_input_channel_throttle_old = receiver_input_channel_throttle_new;
    }
  }

  if (PIND & B00001000) {
    if (last_channel_roll == 0) {
      last_channel_roll = 1;
      timer_roll = current_time;
    }
  }
  else if (last_channel_roll == 1) {
    last_channel_roll = 0;
    receiver_input_channel_roll_new = current_time - timer_roll;
    if ((receiver_input_channel_roll_new > REC_MAX) || (receiver_input_channel_roll_new < REC_MIN)) {
      receiver_input_channel_roll = receiver_input_channel_roll_old;
    } else {
      receiver_input_channel_roll = receiver_input_channel_roll_new;
      receiver_input_channel_roll_old = receiver_input_channel_roll_new;
    }
  }

  if (PIND & B00010000) {
    if (last_channel_pitch == 0) {
      last_channel_pitch = 1;
      timer_pitch = current_time;
    }
  }
  else if (last_channel_pitch == 1) {
    last_channel_pitch = 0;
    receiver_input_channel_pitch_new = current_time - timer_pitch;
    if ((receiver_input_channel_pitch_new > REC_MAX) || (receiver_input_channel_pitch_new < REC_MIN)) {
      receiver_input_channel_pitch = receiver_input_channel_pitch_old;
    } else {
      receiver_input_channel_pitch = receiver_input_channel_pitch_new;
      receiver_input_channel_pitch_old = receiver_input_channel_pitch_new;
    }
  }

  if (PIND & B00100000) {
    if (last_channel_yaw == 0) {
      last_channel_yaw = 1;
      timer_yaw = current_time;
    }
  }
  else if (last_channel_yaw == 1) {
    last_channel_yaw = 0;
    receiver_input_channel_yaw_new = current_time - timer_yaw;
    if ((receiver_input_channel_yaw_new > REC_MAX) || (receiver_input_channel_yaw_new < REC_MIN)) {
      receiver_input_channel_yaw = receiver_input_channel_yaw_old;
    } else {
      receiver_input_channel_yaw = receiver_input_channel_yaw_new;
      receiver_input_channel_yaw_old = receiver_input_channel_yaw_new;
    }
    
  }

  if (PIND & B01000000) {
    if (last_channel_gear == 0) {
      last_channel_gear = 1;
      timer_gear = current_time;
    }
  }
  else if (last_channel_gear == 1) {
    last_channel_gear = 0;
    receiver_input_channel_gear_new = current_time - timer_gear;
    if ((receiver_input_channel_gear_new > REC_MAX) || (receiver_input_channel_gear_new < REC_MIN)) {
      receiver_input_channel_gear = receiver_input_channel_gear_old;
    } else {
      receiver_input_channel_gear = receiver_input_channel_gear_new;
      receiver_input_channel_gear_old = receiver_input_channel_gear_new;
    }
  }
  
}

