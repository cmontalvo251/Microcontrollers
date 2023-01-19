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
//Rev 2.4 - HOLD Mode gains tuned on ground. Ready for Prop Test - 9/27/2017

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

//FORKED CODE TO INNERLOOP OUTERLOOP CONTROLLER WITH UNO
//Rev 4.0 - Pulled out all code that wasn't necessary and changed names of numerous variables
//Rev 4.1 - Tested the receiver signals.
//Rev 4.2 - Tested the IMU readings as well as Pressure readings
//Rev 4.3 - Naze is now reading receiver signals from Arduino.
//Rev 4.4 - Arduino Command Added to send constant command when coded. Software is ready for flight test.
//Rev 4.5 - Added Throttle Hover and Throttle constant angle --- 12/12/2018 - Flight test successful on 12/14/2018
//Rev 4.6 - Added Altitude Hold Mode using Barometer - Would rather wait until I have datalogging so I can plot some of these things during flight but just gonna try it. 
//Would also like to see the use of the TOFS - 12/14/2018 - Flight test 12/15/2018 failed. Moving on to DUE with Datalogging

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
int THROTTLE_CMD,AILERON_CMD,RUDDER_CMD,ELEVATOR_CMD;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;
boolean QUADARMED,ARDUINO_CMD;
int MODE = 0;
float int_pitch=0,int_roll=0;

//////////////USER PARAMETERS////////////////

#define PRINTSERIAL false //set to false when doing flight tests but true when doing classroom demos
#define REC_MIN 1000
#define REC_MAX 2000
#define NEXT_TIME 0.1

#define PWM_MIN 1136.0
#define PWM_MID 1526.0
#define PWM_MAX 1908.0
#define PHI_MAX 55.0
#define THR_HOVER 1648.0

#define ALTITUDE_COMMAND_PRESSURE 0.1
#define KI_ALTITUDE 0
#define KP_ALTITUDE 0

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
Servo THROTTLE;
Servo AILERON;
Servo RUDDER;
Servo ELEVATOR;
Servo GEAR;

float roll=0,pitch=0,yaw=0,roll_rate=0,pitch_rate=0,yaw_rate=0;
float pitch_command,roll_command;
imu::Vector<3> euler;
imu::Vector<3> rate_fc;
imu::Vector<3> euler_prev;
imu::Vector<3> rate_fc_prev;
imu::Vector<3> lin_accel;
float getEulerTime = 0;
float pressureKPA = 0;
float delP = 0,delZ = 0;
float Az = 0;
float alt_int = 0;

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

  //Take like 100 pressure readings and average them
  int N = 100;
  for (int i = 0;i<N;i++) {
   pressureKPA += mpl115a2.getPressure();
  }
  pressureKPA/=float(N);
  
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
  PCMSK2 |= (1 << PCINT22); //Pin 6 - AUX1
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Attach COMMANDS to Send to NAZE
  THROTTLE.attach(13);  //12
  AILERON.attach(12); 
  ELEVATOR.attach(11); 
  RUDDER.attach(10); 
  GEAR.attach(8);

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

  if ((receiver_input_channel_gear < 1200) and (receiver_input_channel_gear > 1000)) {
    ARDUINO_CMD = true;
    alt_int+= delZ; //As I climb this number will get bigger but it will accumulate over time
  } else {
    ARDUINO_CMD = false;
    alt_int = 0; //Anti-Windup
  }

  ///Get Euler Angles and Pressure Data
  if ((millis()/1000.0-getEulerTime) > NEXT_TIME) {
    rate_fc = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    getEulerTime = millis()/1000.0 + NEXT_TIME;
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    //lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    //Need to verify this is right simply through experimentation
    //Verified - 12/6/2018
    Az = -lin_accel.z()+9.81;
    roll_rate = -PI/180.0*rate_fc.y();
    pitch_rate = -PI/180.0*rate_fc.x();
    yaw_rate = -PI/180.0*rate_fc.z();
    roll = PI/180.0*(euler.y()); //The 9.0 is because the sensor is not mounted flat on the PCB
    pitch = PI/180.0*(euler.z()-9);
    yaw = PI/180.0*euler.x();
    //Grab pressure data
    delP = -(mpl115a2.getPressure()-pressureKPA); ///If I climb in altitude getPressure will be smaller than pressureKPA. This means that delP will be positive as I climb in altitude
    delZ = delP - ALTITUDE_COMMAND_PRESSURE; //So in order to be off the ground though I need to have delP be slightly positive otherwise I'll just crash straight into the ground
  }

  if (!ARDUINO_CMD) {
    THROTTLE_CMD = receiver_input_channel_throttle;
    AILERON_CMD = receiver_input_channel_roll;
    ELEVATOR_CMD = receiver_input_channel_pitch;
    RUDDER_CMD = receiver_input_channel_yaw;
  } else {
    //Test Number 1 - Send constant throttle command
    //THROTTLE_CMD = THR_HOVER;
    //AILERON_CMD = receiver_input_channel_roll;
    //ELEVATOR_CMD = receiver_input_channel_pitch;
    //RUDDER_CMD = receiver_input_channel_yaw;

    //Test Number 2 - Send 20 degree bank angle command
    //float PHI_CMD = 20.0;
    //float m = 2.0*PHI_MAX/(PWM_MAX - PWM_MIN);
    //AILERON_CMD = PHI_CMD/m + PWM_MID;
    //THROTTLE_CMD = receiver_input_channel_throttle;
    //ELEVATOR_CMD = receiver_input_channel_pitch;
    //RUDDER_CMD = receiver_input_channel_yaw;

    //Test Number 3 - Try and get the Quad to maintain an equilibrium altitude
    THROTTLE_CMD = THR_HOVER - KI_ALTITUDE*alt_int - KP_ALTITUDE*delZ; //if alt_int is positive that means I'm over the altitude COMMAND PRESSURE which means I need to reduce throttle.
    AILERON_CMD = receiver_input_channel_roll;
    ELEVATOR_CMD = receiver_input_channel_pitch;
    RUDDER_CMD = receiver_input_channel_yaw;
  }

  //Send Command to NAZE
  THROTTLE.writeMicroseconds(THROTTLE_CMD);
  AILERON.writeMicroseconds(AILERON_CMD);
  ELEVATOR.writeMicroseconds(ELEVATOR_CMD);
  RUDDER.writeMicroseconds(RUDDER_CMD);
  GEAR.writeMicroseconds(receiver_input_channel_gear);
  
  //Print Controller Health
  #if PRINTSERIAL
    Serial.print("T=");
    Serial.print(THROTTLE_CMD);
    Serial.print(" ");

    //Serial.print("Y=");
    //Serial.print(RUDDER_CMD);
    //Serial.print(" ");
    
    //Serial.print("P=");
    //Serial.print(ELEVATOR_CMD);
    //Serial.print(" ");
  
    //Serial.print("R=");
    //Serial.print(AILERON_CMD);
    //Serial.print(" ");

    //Serial.print("G=");
    //Serial.print(receiver_input_channel_gear);
    //Serial.print(" ");
    
    //Serial.print("RPY=");
    //Serial.print(roll);
    //Serial.print(" ");
    
    //Serial.print(pitch);
    //Serial.print(" ");

    //Serial.print(yaw);
    //Serial.print(" ");

    //Serial.print(roll_rate);
    //Serial.print(" ");

    //Serial.print(pitch_rate);
    //Serial.print(" ");

    //Serial.print(yaw_rate);
    //Serial.print(" ");

    Serial.print("dP=");
    Serial.print(delP);
    Serial.print(" ");

    Serial.print("delZ=");
    Serial.print(delZ);
    Serial.print(" ");

    Serial.print("alt_int=");
    Serial.print(alt_int);
    Serial.print(" ");

    //Serial.print("Az=");
    //Serial.print(Az);

    Serial.print("\n");
  #endif
  
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

