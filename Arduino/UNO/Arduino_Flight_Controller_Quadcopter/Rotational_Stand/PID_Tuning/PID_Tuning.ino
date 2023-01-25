char val[15]; //Data received from the serial port
char throttle_str[4]; //4 digit throttle command sent to motors in string
#define THROTTLE_MAX 1250
#define THROTTLE_OFF 1100
#define THROTTLE_MIN 1200

int nominal_throttle = THROTTLE_OFF;

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

imu::Vector<3> euler;
imu::Vector<3> rate;

float kp=0,kd=0,ki=0,pitch_int=0;

#include <Servo.h>

Servo esc[4];
int motors[4];

void setup() {
  Serial.begin(9600);
  for (int idx = 0;idx<4;idx++) {
    motors[idx] = 0;
    //Using CleanFlight Notation
    esc[idx].attach(11-idx);
  }

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
    val[0] = '1';
    int length_of_str = Serial.available();
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
      ctr++;
      delay(10);
    }

    //If val starts with a zero it means that we are shutting everything down
    if (val[0] == '0') {
      nominal_throttle = THROTTLE_OFF;
      Serial.println("SHUTTING DOWN!!!");
      kp = 0;
      kd = 0;
      ki = 0;
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
    } else {
      //Interpret as a 4 digit throttle command
      for (int idx = 0;idx<4;idx++){
        throttle_str[idx] = val[idx];
      }
      nominal_throttle = atoi(throttle_str);
    }
  }
  //////////////////////////////////

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
  pitch_rate = DeadBand(pitch_rate,0.3);
  ///////////////////////////////////////

  ////PID NOTES/////
  //Nominal of 1250 is good
  //Looks like kd = 0.5 is good
  //Kp = 3 seems like it works well
  //For right now I def need ki. There is too much weight on the rear. That's why the back two motors are working so hard.
  //Maybe I can load the front? Can't find anything. Let's just try and tune I.
  //Looks like an I of 0.5 is just fine but it may be too high.
  //Ok so Nom = 1250, kp = 3, kd = 0.5, ki = 0.5
  //12/28/31 - Checked the battery voltage and it was 15.77 the gains above did not work.
  //I'm thinking it's just battery sag from the voltage. Let's try 1280 and see if it works.
  
  
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
  esc[0].writeMicroseconds(throttle_rear);
  esc[1].writeMicroseconds(throttle_front);
  esc[2].writeMicroseconds(throttle_rear);
  esc[3].writeMicroseconds(throttle_front);
  
  ////SERIAL PRINTS//
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.print(kp);
  Serial.print(" ");
  Serial.print(kd);
  Serial.print(" ");
  Serial.print(ki);
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

float DeadBand(float input,float threshold) {
  float output = input;
  if (abs(input) <= threshold) {
    output = 0;
  } else {
    output += -sign(input)*threshold;
  }
  return output;
}

int sign(float val) {
 if (val < 0) return -1;
 if (val==0) return 0;
 return 1;
}

/////// ADD THIS IN ////////////
//  
//  //The refresh rate is 250Hz. That means the esc's need there pulse every 4ms.
//  while(micros() - loop_timer < 4000);                                      //We wait until 4000us are passed.
//  loop_timer = micros();                                                    //Set the timer for the next loop.
//
//  PORTD |= B11110000;                                                       //Set digital outputs 4,5,6 and 7 high.
//  timer_channel_1 = esc_1 + loop_timer;                                     //Calculate the time of the faling edge of the esc-1 pulse.
//  timer_channel_2 = esc_2 + loop_timer;                                     //Calculate the time of the faling edge of the esc-2 pulse.
//  timer_channel_3 = esc_3 + loop_timer;                                     //Calculate the time of the faling edge of the esc-3 pulse.
//  timer_channel_4 = esc_4 + loop_timer;                                     //Calculate the time of the faling edge of the esc-4 pulse.
//  
//  //There is always 1000us of spare time. So let's do something usefull that is very time consuming.
//  //Get the current gyro and receiver data and scale it to degrees per second for the pid calculations.
//  gyro_signalen();
//
//  while(PORTD >= 16){                                                       //Stay in this loop until output 4,5,6 and 7 are low.
//    esc_loop_timer = micros();                                              //Read the current time.
//    if(timer_channel_1 <= esc_loop_timer)PORTD &= B11101111;                //Set digital output 4 to low if the time is expired.
//    if(timer_channel_2 <= esc_loop_timer)PORTD &= B11011111;                //Set digital output 5 to low if the time is expired.
//    if(timer_channel_3 <= esc_loop_timer)PORTD &= B10111111;                //Set digital output 6 to low if the time is expired.
//    if(timer_channel_4 <= esc_loop_timer)PORTD &= B01111111;                //Set digital output 7 to low if the time is expired.
//  }
//}

