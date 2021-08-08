char val[15]; //Data received from the serial port
char throttle_str[4]; //4 digit throttle command sent to motors in string
#define THROTTLE_MAX 1900
#define THROTTLE_OFF 1100
#define THROTTLE_MIN 1200
#define RECEIVER_MIN 1100
#define RECEIVER_MAX 1900
#define RECEIVER_MID 1500
#define ANGLE_MAX 30.0

int nominal_throttle = THROTTLE_OFF;
float pitch_rate_prev = 0;

//SD Card Stuff//
#include <SPI.h>
#include <SD.h>
//Make sure to select Adafruit Feather M0 - If you don't have that option you downloaded the wrong board.
//Make sure to unplug the battery before flashing code.
//for some reason the serial read isn't working for 1 number so the throttle switch is used to arm. 
//Make sure to have throttle down when plugging in the battery!!!!
File logfile;
float lastPrint = 0;

//IMU//
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;

float kp=0,kd=0,ki=0,pitch_int=0;
bool QUAD_ARMED = false;

#include <Servo.h>

Servo esc[2];

void setup() {
  Serial.begin(9600);

  if (!SD.begin(4)) {      // 
    Serial.println("Card init. failed!");
    while(1){};
  }
  char filename[15];
  strcpy(filename, "RTSMZ000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[5] = '0' + i/100;
    filename[6] = '0' + (i/10)%10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while(1){};
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  //Attach ESCs
  //Just need left and right motor
  esc[0].attach(9);
  esc[1].attach(10);

  ///Receiver Stuff
  //https://playground.arduino.cc/Main/PcInt
  //PCICR |= (1 << PCIE2); 
  //PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  //PCMSK2 |= (1 << PCINT20); //Pin 4 - Pitch
  
  //zero_timer = micros();   //Set the zero_timer for the first loop.

  //Initialize Command Signals
  receiver_input_channel_throttle = 1100;
  receiver_input_channel_pitch = 1500;
  //Setup pulseins
  //pinMode(12,INPUT);
  //pinMode(13,INPUT);

  // IMU //
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  
  //Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
  
  delay(1000);
    
  //bno.setExtCrystalUse(true);

  lastPrint = millis()/1000.0;
}

void loop() {
  //Check For Inputs from the User
  ///////////////////////////////
  if (Serial.available() > 0) {
    Serial.print("Serial Available!!!\n");
    for (int idx = 0;idx<5;idx++){
      val[idx] = '0';
    }
    val[0] = '9';
    int length_of_str = Serial.available()-1; //The minus 1 is here because of \n or newline
    Serial.print("Length of String = ");
    Serial.println(length_of_str);
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
      if (ctr < length_of_str) {
        Serial.print(val[ctr]);
      }
      Serial.print(":");
      ctr++;
      delay(10);
    }

    //If val is just one number it means we're starting or shutting down the quad
    if (length_of_str == 1) {
      if (val[0] == '0') {
        QUAD_ARMED = false;
        Serial.println("SHUTTING DOWN!!!"); 
        kp = 0;
        kd = 0;
        ki = 0;
        receiver_input_channel_throttle = 0;
      } else if (val[0] == '1') {
        QUAD_ARMED = true;
        Serial.println("ARMING Stand!!");
      }
    } else if (val[0] == 'k') {
      Serial.println("Changing Gains");
        //it means we're changing a gain
        if (val[1] == 'p') {
          //Proportional Gain
          Serial.println("Proportional");
          kp = getFloat(val,length_of_str);
        } else if (val[1] == 'd') {
          //Derivative Gain
          Serial.println("Derivative");
          kd = getFloat(val,length_of_str);
        } else if (val[1] == 'i') {
          //Integral Gain
          Serial.println("Integral");
          ki = getFloat(val,length_of_str);
        }
    } else if (length_of_str == 4) {
      //This means we're changing the nominal throttle
      Serial.println("Changing Throttle");
      char throttle_str[4];
      for (int idx = 0;idx<4;idx++) {
        throttle_str[idx] = val[idx];
      }
      if (QUAD_ARMED) {
        receiver_input_channel_throttle = atoi(throttle_str);
      }
    }
  }
  //////////////////////////////////

  if (QUAD_ARMED) {
    //receiver_input_channel_throttle = DeadBand(receiver_input_channel_throttle,10,RECEIVER_MIN);
    nominal_throttle = receiver_input_channel_throttle;
  } else {
    nominal_throttle = THROTTLE_OFF;
  }

  //////SATURATION BLOCK///////////////
  if (nominal_throttle > THROTTLE_MAX) {
    nominal_throttle = THROTTLE_MAX;
  }
  if (nominal_throttle < THROTTLE_OFF) {
    nominal_throttle = THROTTLE_OFF;
  }
  ////////////////////////////////////

  ///////Get Angular Rates///////////////
   /* Get a new sensor event */ 
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp); 
  //Convert Pitch angle using Accelerometer
  //float roll = atan2(a.acceleration.y,a.acceleration.z);
  float pitch = 180.0/PI*atan2(-a.acceleration.x,a.acceleration.z); //Modified version of pitch since roll angle is always zero for this platform
  float pitch_rate = (-g.gyro.y)*0.5 + 0.5*pitch_rate_prev; //and degree/sec
  pitch_rate_prev = pitch_rate;
  ///////////////////////////////////////

  //////FIX DERIVATIVE JITTER ISSUE//////
  pitch_rate = DeadBand(pitch_rate,0.3,0.0);
  ///////////////////////////////////////

  ////PID NOTES/////
  //Nominal of 1250 is good too
  //Looks like kd = 0.5 is good
  //Kp = 3 seems like it works well.
  //For right now I def need ki. There is too much weight on the rear. That's why the back two motors are working so hard.
  //Maybe I can load the front? Can't find anything. Let's just try and tune I.
  //Looks like an I of 0.5 is just fine but it may be too high.

  //If the battery sags you need to increase throttle. Using a fully charged battery on 12/30/2017 these gains and nominal control worked.
  //Nominal = 1268
  //kp = 3.0, kd = 0.5, ki = 0.2
  //Note I needed to put an extra battery on the front of the quad so it wasn't so back heavy.
  
  /////////PID CONTROLLER//////////////
  
  ///GET PITCH COMMAND FROM RECEIVER/////
  //Ok this code works and is ready for class. The problem is the super high up cg. Definitely need a non-linear effector like perhaps
  //divide by cos(theta) or something like that to jack up the gains. Any way the next steps are to bring this to work and show the class.
  //Then the step after that is to repair the racing quad so you can actually use it for what it was intended to do.
  //float pitch_command = -ANGLE_MAX/(RECEIVER_MAX-RECEIVER_MID)*(receiver_input_channel_pitch-RECEIVER_MID);
  //pitch_command = DeadBand(pitch_command,0.3,0.0);
  float pitch_command = 0;

  /////ERROR SIGNALS/////
  float error = pitch - pitch_command;
  float pitch_effort = kp*error + kd*pitch_rate + ki*pitch_int;

  //Integral Gains
  if (abs(ki) > 0) {
    pitch_int += error*0.06; //That 0.06 could change depending on how fast the code runs.
  } else {
    //Anti Wind up
    pitch_int = 0;
  }
  float throttle_left = nominal_throttle + pitch_effort;
  float throttle_right = nominal_throttle - pitch_effort;
  //////////////////////////////////////

  ////SATURATION BLOCK ROUND 2//////////
  if (throttle_left < THROTTLE_OFF) {
    throttle_left = THROTTLE_OFF;
  } else if (throttle_left > THROTTLE_MAX) {
    throttle_left = THROTTLE_MAX;
    //Anti Windup
    pitch_int = anti_windup(pitch_int,error);
  }
  if (throttle_right < THROTTLE_OFF) {
    throttle_right = THROTTLE_OFF;
  } else if (throttle_right > THROTTLE_MAX) {
    throttle_right = THROTTLE_MAX;
    //Anti Windup
    pitch_int = anti_windup(pitch_int,error);
  }

  ////SATURATION BLOCK ROUND 3//////
  if (nominal_throttle > THROTTLE_MIN) {
    if (throttle_left < THROTTLE_MIN) {
      throttle_left = THROTTLE_MIN;
    }
    if (throttle_right < THROTTLE_MIN) {
      throttle_right = THROTTLE_MIN;
    }
  }

  //Throttle Commands
  if (QUAD_ARMED) {
    esc[0].writeMicroseconds(throttle_left);
    esc[1].writeMicroseconds(throttle_right);
  } else {
    for (int idx = 0;idx<2;idx++) {
      esc[idx].writeMicroseconds(THROTTLE_OFF);
    }
  }
  
  ////SERIAL PRINTS//
  if (millis()/1000.0 > 0.5 + lastPrint) {

    //Reset Last Print
    lastPrint = millis()/1000.0;
    if (QUAD_ARMED) {
       Serial.print("!!! ");
    }
    Serial.print(millis()/1000.0);
    Serial.print(" ");
    Serial.print(kp);
    Serial.print(" ");
    Serial.print(kd);
    Serial.print(" ");
    Serial.print(ki);
    Serial.print(" ");
    Serial.print(receiver_input_channel_throttle);
    Serial.print(" ");
    Serial.print(nominal_throttle);
    Serial.print(" ");
    Serial.print(throttle_left);
    Serial.print(" ");
    Serial.print(throttle_right);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.print(pitch_rate);
    Serial.print(" ");
    Serial.print(pitch_int);
    Serial.println(" ");
  }
}

float anti_windup(float eint,float e) {
  //If eint and e are the same sign then substract off the error otherwise leave it
  if (sign(eint) == sign(e)) {
    eint -= e*0.06;
  }
  return eint;
}

float getFloat(char* val,int length_of_str) {
    //Alright first things first we need to copy the contents of one string into another.
    char float_str[13];
    for (int idx = 2;idx<length_of_str;idx++) {
      float_str[idx-2] = val[idx]; //Start at 2 because the first two are letters
    }
    //Place a Decimal Point at the end.
    String str = String(float_str);
    return str.toFloat();
}

float DeadBand(float input,float threshold,float offset) {
  float output = input - offset;
  if (abs(input) <= threshold) {
    output = 0;
  } else {
    output += -sign(input)*threshold;
  }
  return output+offset;
}

int sign(float val) {
 if (val < 0) return -1;
 if (val==0) return 0;
 return 1;
}
