char val[15]; //Data received from the serial port
char throttle_str[4]; //4 digit throttle command sent to motors in string
#define THROTTLE_MAX 1900
#define THROTTLE_OFF 1100
#define THROTTLE_MIN 1200
#define RECEIVER_MIN 1100
#define RECEIVER_MAX 1900

int nominal_throttle = THROTTLE_OFF;

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

imu::Vector<3> euler;
imu::Vector<3> rate;

byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;

float kp=0,kd=0,ki=0,pitch_int=0;
bool QUAD_ARMED = false;

#include <Servo.h>

Servo esc[4];
int motors[4];

void setup() {
  Serial.begin(9600);

  //Attach ESCs
  for (int idx = 0;idx<4;idx++) {
    motors[idx] = 0;
    //Using CleanFlight Notation
    esc[idx].attach(11-idx);
  }

  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 0;

  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}

void loop() {
  //Check For Inputs from the User
  ///////////////////////////////
  if (Serial.available() > 0) {
    for (int idx = 0;idx<5;idx++){
      val[idx] = '0';
    }
    val[0] = '9';
    int length_of_str = Serial.available();
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
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
      } else if (val[0] == '1') {
        if (receiver_input_channel_throttle < 1120) {
          QUAD_ARMED = true;
          Serial.println("ARMING QUAD!!");
        } else {
          Serial.println("THROTTLE WARNING - REDUCE THROTTLE");
        }
      }
    } else if (val[0] == 'k') {
        //it means we're changing a gain
        if (val[1] == 'p') {
          //Proportional Gain
          kp = getFloat(val,length_of_str);
        } else if (val[1] == 'd') {
          //Derivative Gain
          kd = getFloat(val,length_of_str);
        } else if (val[1] == 'i') {
          //Integral Gain
          ki = getFloat(val,length_of_str);
        }
    }
  }
  //////////////////////////////////

  if (QUAD_ARMED) {
    //receiver_input_channel_throttle = DeadBand(receiver_input_channel_throttle,10,RECEIVER_MIN);
    nominal_throttle = throttle_curve(receiver_input_channel_throttle);
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
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  float pitch = euler.y(); //Both of these are in degrees 
  float pitch_rate = -rate.y(); //and degree/sec
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

  //NEXT STEPS - Add pitch_command control using the pitch stick. Once it's stable try and move the quad to different angles and see if it can do it.
  //Set the MAX pitch command to 5 degrees so you don't twitch the thing like crazy. Then move it up from there.
  
  /////////PID CONTROLLER//////////////
  float error = pitch - 0;
  float pitch_effort = kp*(pitch-0) + kd*(pitch_rate-0) + ki*pitch_int;

  if (abs(ki) > 0) {
    pitch_int += error*0.06; //That 0.06 could change depending on how fast the code runs.
  } else {
    pitch_int = 0;
  }
  float throttle_rear = nominal_throttle + pitch_effort;
  float throttle_front = nominal_throttle - pitch_effort;
  //////////////////////////////////////

  ////SATURATION BLOCK ROUND 2//////////
  if (throttle_rear < THROTTLE_OFF) {
    throttle_rear = THROTTLE_OFF;
  } else if (throttle_rear > THROTTLE_MAX) {
    throttle_rear = THROTTLE_MAX;
    //Anti Windup
    if (abs(ki) > 0) {
      pitch_int -= error*0.06;
    }
  }
  if (throttle_front < THROTTLE_OFF) {
    throttle_front = THROTTLE_OFF;
  } else if (throttle_front > THROTTLE_MAX) {
    throttle_front = THROTTLE_MAX;
    //Anti Windup
    if (abs(ki) > 0) {
      pitch_int -= error*0.06;
    }
  }

  ////SATURATION BLOCK ROUND 3//////
  if (nominal_throttle > THROTTLE_MIN) {
    if (throttle_rear < THROTTLE_MIN) {
      throttle_rear = THROTTLE_MIN;
    }
    if (throttle_front < THROTTLE_MIN) {
      throttle_front = THROTTLE_MIN;
    }
  }

  //Throttle Commands
  if (QUAD_ARMED) {
    esc[0].writeMicroseconds(throttle_rear);
    esc[1].writeMicroseconds(throttle_front);
    esc[2].writeMicroseconds(throttle_rear);
    esc[3].writeMicroseconds(throttle_front);
  } else {
    for (int idx = 0;idx<4;idx++) {
      esc[idx].writeMicroseconds(THROTTLE_OFF);
    }
  }
  
  ////SERIAL PRINTS//
  if (QUAD_ARMED) {
     Serial.print("ARMED!!! ");
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
  Serial.print(throttle_rear);
  Serial.print(" ");
  Serial.print(throttle_front);
  Serial.print(" ");
  Serial.print(pitch_int);
  Serial.print(" ");
  Serial.print(pitch);
  Serial.print(" ");
  Serial.print(pitch_rate);
  Serial.println(" ");
  ////////////////////////////////////
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

int throttle_curve(int receiver_signal) {
  float slope = float(THROTTLE_MAX-THROTTLE_OFF)/float(RECEIVER_MAX-RECEIVER_MIN);
  return int(THROTTLE_OFF + slope*(float(receiver_signal)-RECEIVER_MIN));
}

int sign(float val) {
 if (val < 0) return -1;
 if (val==0) return 0;
 return 1;
}

///INTERUPPTS FOR RECEIVER SIGNALS
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
  }  
}

