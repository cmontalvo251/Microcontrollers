#include "vectorTypes.h"
#include "receiver.h"
#include "debug.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h> //The above lines are calling all necessary libraries and accessory code.

Adafruit_BNO055 bno = Adafruit_BNO055(55);

unsigned long LastOutTime = 0;     // Output - Pwm to Motors, 
const int OutPeriod = 5;          // 200Hz = 5ms

unsigned long LastCmdTime = 0;     // Commands - (Reciver)
const int CmdPeriod = 100;          // 10Hz = 100ms

unsigned long LastIMUTime = 0;     // IMU
const int IMUPeriod = 5;          // 200Hz = 5ms

//#define IMUPRINT //Comment out this line if you don't want to see any print statements
//#define RXPRINT
//#define MOTORPRINT

#define CONSTRAIN(in,min,max) (in > max ? max : (in < min ? min : in))
#define DEG2RAD PI/180.0

#include <Servo.h>
Servo motor1,motor2,motor3,motor4;

int potpin = 0;
float pot;

float Roll_cmd;
float Pitch_cmd;
float yaw_rx;
float thrust_rx;
float aux0_rx;
float aux1_rx;

float perrorIntegral,qerrorIntegral,rerrorIntegral;

float tau_yaw, Yaw_old, Yaw_cmd, tau_yawdt;
float tau_roll, tau_rolldt;
float tau_pitch, tau_pitchdt;

float cal_yaw, cal_roll, cal_pitch;
int cal_int;
int thrust;
int state = 0;
int motor1Pin = 6, motor2Pin = 9 ,motor3Pin = 7 ,motor4Pin = 8; 
int Motor1PWM,Motor2PWM,Motor3PWM,Motor4PWM; //The above lines are declaring variables and values that will be used throughout the code.
void setup() 
{
  //Receiver.h has a namespace can we run the init?
  receiverModule::init(); ////Not entirely sure why KyrellGod uses a namespace instead of a class
  delay(250); ////but head over to receiver.cpp to see the namespace functions
  receiverModule::resetLimitStats();

  //Setup the motors
  motor1.attach(motor1Pin);
  motor2.attach(motor2Pin);
  motor3.attach(motor3Pin);
  motor4.attach(motor4Pin);
  //pinMode(motor1Pin,OUTPUT);
  //pinMode(motor2Pin,OUTPUT);
  //pinMode(motor3Pin,OUTPUT);
  //pinMode(motor4Pin,OUTPUT);
  //Change the output resolution
  //analogWriteResolution(12); //Function only used for an Arduino Due.
  //Wait 5 ms seconds?
  delay(5);
  //Send a minimum signal to the motors
  //analogWrite(motor1Pin,1000);
  //analogWrite(motor2Pin,1000);
  //analogWrite(motor3Pin,1000);
  //analogWrite(motor4Pin,1000); //The above lines are setting the already declared variables as ouput pins and making them write an analog value to that pin.
  //Delay 500 ms
  motor1.writeMicroseconds(1000);
  motor2.writeMicroseconds(1000);
  motor3.writeMicroseconds(1000);
  motor4.writeMicroseconds(1000);
  delay(500);

  //Begin Serial. - Probable want to turn this off once we get going
  Serial.begin(115200);

  //Turn on the IMU
  if(!bno.begin())
  { 
    Serial.print("Oops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  Yaw_old = 0; //The above lines are used to initiate the IMU(9dof sensor)

  //Save the current time
  LastCmdTime = millis();
  LastOutTime = millis(); 

  Serial.print("All good. Time to fly \n");

}

void readIMU() //Takes raw data from the IMU and converts to readable data and prints it to the serial monitor.
{  
  imu::Quaternion q = bno.getQuat();
  //q.normalize();
  //float temp = q.x();  q.x() = -q.y();  q.y() = temp;
  //q.z() = -q.z();

  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  //Serial.print("X: ");
  //Serial.print(event.orientation.x, 4);
  //Serial.print("\tY: ");
  //Serial.print(event.orientation.y, 4);
  //Serial.print("\tZ: ");
  //Serial.print(event.orientation.z, 4);
  //Serial.println("");

  tau_yaw = event.orientation.x;
  tau_roll = event.orientation.y;
  tau_pitch = event.orientation.z;
  
  //imu::Vector<3> euler = q.toEuler();
  //tau_roll = 180/M_PI * euler.z();
  //tau_pitch = -180/M_PI * euler.y();
  //tau_yaw = 180/M_PI * euler.x();
  
  #ifdef IMUPRINT
  Serial.print("Roll: ");
  Serial.print(tau_roll);  // pitch, nose-down is positive, x-axis points right
  Serial.print("\t Pitch: ");
  Serial.print(tau_pitch);
  Serial.print("\t Yaw: ");
  Serial.print(tau_yaw);
  #endif

  imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  tau_rolldt = -rate.y() * 180.0/M_PI;
  tau_pitchdt = -rate.x() * 180.0/M_PI;
  tau_yawdt = -rate.z() * 180.0/M_PI;
  
  #ifdef IMUPRINT
  Serial.print("\t Roll Rate: ");
  Serial.print(tau_rolldt);
  Serial.print("\t Pitch Rate: ");
  Serial.print(tau_pitchdt);
  Serial.print("\t Yaw Rate: ");
  Serial.println(tau_yawdt);
  #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readReceiver() //Maps the throttle values to PWM values for the receiver to read.
{
  float pitchrc = receiverModule::getClippedChannelValue_f(THROTTLE);
  Pitch_cmd = -2*(50.0/590.0)*(pitchrc-1503.0);
  float rollrc = receiverModule::getClippedChannelValue_f(ELEVATOR);
  Roll_cmd = -2*(50.0/590.0)*(rollrc-1502.0);
  float yawrc = receiverModule::getClippedChannelValue_f(RUDDER);
  yaw_rx = -2*(135.0/590.0)*(yawrc-1502.0); //this is really a yawRate command
  thrust_rx = receiverModule::getClippedChannelValue_f(AILERON);
  aux0_rx = receiverModule::getClippedChannelValue_f(AUX0);
  //aux1_rx = receiverModule::getClippedChannelValue_f(AUX1);

  #ifdef RXPRINT
  //DEBUG Reciver:
  Serial.print("Time: ");
  Serial.print(LastOutTime);
  Serial.print("us ");

  Serial.print("\t T: ");
  Serial.print(thrust_rx);
  Serial.print("us ");

  Serial.print("\t Y: ");
  Serial.print(yaw_rx); //Yaw data
  Serial.print("deg/s ");
    
  Serial.print("\t R: ");
  Serial.print(Roll_cmd); //Roll data
  Serial.print("deg ");
    
  Serial.print("\t P: ");
  Serial.print(Pitch_cmd); //Pitch data
  Serial.print("deg");

  Serial.print("\ Aux: ");
  Serial.print(aux0_rx);
  Serial.print(" us");
  Serial.println();
  #endif
}

void callStablizer(float elapsedTime) //Takes the PID control system and use the control constants to control the throttle output
{
  //3.0, 0.9, 0.5, 1.0 - not enough control authority so doubling it.
  //6.0, 1.8, 1.0, 2.0 - yaw seems backwards and roll and pitch oscillations really bad
  //4.0, 1.2, 0.8, -4.0 - Yaw channel is good now. roll and pitch still oscillating. put back to normal
  //3.0, 0.9, 0.5, -4.0 - Flight for 10 seconds seems ok but after 12 seconds it flips a shit. Seems like we should remove ki.
  //3.0, 0.9, 0.0, -4.0 - Flight was wayyyy better. Suggest increasing kp to get more authority.
  //3.0, 0.9, 0.05, -4.0 - Flight was ok ish. Still think it needs more kp
  //5.0, 0.9, 0.05, -4.0 - Copter flipped over. Not sure what to do.
  //3.0, 0.9, 0.001, -4.0 - Still needs more control - maybe more kd? and more kyawy?
  //3.0, 2.0, 0.001, -8.0 - Yaw channel seems wrong. Maybe flip it?
  //5.0, 0.9, 0.0, 8.0 - noticed a preference to roll in one direction over the other maybe need lots of integral gain?
  //5.0, 0.9, 1.0, 8.0 - ok yaw channel was right before. Change it back
  //5.0, 0.9, 1.0, -6.0 - ok integral gain is too high and it lags too much. Try removing ki again and increasing kd
  //5.0, 2.0, 0.0, -6.0 - couldn't yaw in one direction.

  ///ALL THIS ABOVE IS SHIT BECAUSE THE SENSOR WAS NOT WORKING RIGHT. WE MOVED FROM QUATS TO EULERS NATIVELY
  
  //3.0, 2.0, 0.0, -6.0 - not enough control authority. Add some integral gain
  //3.0, 2.0, 0.05, -6.0 - buried the stick and still no response. Permanently grounding vehicle. Suggest upgrading FC and diagnose motor prop combo
  
  float kp = 3.0;
  float kd = 2.0;
  float ki = 0.05;
  float kyaw = -6.0;

  float phidot_command = kp*(Roll_cmd - tau_roll);
  float thetadot_command = kp*(Pitch_cmd - tau_pitch);
  float psidot_command = yaw_rx;
  
  phidot_command = CONSTRAIN(phidot_command,-250.0,250.0);
  thetadot_command = CONSTRAIN(thetadot_command,-250.0,250.0);

  //These ptpdot commands are Euler angle derivatives and need to be converted to body frame angular rates
  float cos_theta = cos(tau_pitch*DEG2RAD);
  float sin_theta = sin(tau_pitch*DEG2RAD);
  float cos_phi = cos(tau_roll*DEG2RAD);
  float sin_phi = sin(tau_roll*DEG2RAD);
  float p_command = phidot_command - sin_theta*psidot_command;
  float q_command = cos_phi*thetadot_command + sin_phi*cos_theta*psidot_command;
  float r_command = -sin_phi*thetadot_command + cos_phi*cos_theta*psidot_command;
  
  float perror = (p_command - tau_rolldt);
  float qerror = (q_command - tau_pitchdt);
  float rerror = (r_command - tau_yawdt);
  
  //Need to compute integral error + anti-Windup
  perrorIntegral += perror*elapsedTime;
  perrorIntegral = CONSTRAIN(perrorIntegral,-50.0,50.0);
  qerrorIntegral += qerror*elapsedTime;
  qerrorIntegral = CONSTRAIN(qerrorIntegral,-50.0,50.0);

  float d_roll = kd*perror + ki*perrorIntegral;
  float d_pitch = kd*qerror + ki*qerrorIntegral;
  float d_yaw = kyaw*rerror;
  Motor3PWM =   thrust_rx + d_roll - d_pitch - d_yaw + 200;
  Motor2PWM =  thrust_rx - d_roll - d_pitch + d_yaw + 200;
  Motor1PWM = thrust_rx - d_roll + d_pitch - d_yaw + 200;
  Motor4PWM =  thrust_rx + d_roll + d_pitch + d_yaw + 200;

  Motor1PWM = CONSTRAIN(Motor1PWM,1200,1900);
  Motor2PWM = CONSTRAIN(Motor2PWM,1200,1900);
  Motor3PWM = CONSTRAIN(Motor3PWM,1200,1900);
  Motor4PWM = CONSTRAIN(Motor4PWM,1200,1900);
}

void callMotors() //Prints the motor PWM signals to the serial monitor and write an analog value to that from one variable to another.
{

     //Uncomment this section to print the values of MotorPWM signals!
     #ifdef MOTORPRINT
     //Serial.print("Motor1: ");
     Serial.print(Motor1PWM);
     Serial.print(" ");
     Serial.print(Motor2PWM);
     Serial.print(" ");
     Serial.print(Motor3PWM);
     Serial.print(" ");
     Serial.print(Motor4PWM);
     Serial.println();
     #endif
     
     //analogWrite(motor1Pin,Motor1PWM);
     //analogWrite(motor2Pin,Motor2PWM);
     //analogWrite(motor3Pin,Motor3PWM);
     //analogWrite(motor4Pin,Motor4PWM);
     if ((aux0_rx < 1500) and (aux0_rx > 1000)) {
      motor1.writeMicroseconds(Motor1PWM);
      motor2.writeMicroseconds(Motor2PWM);
      motor3.writeMicroseconds(Motor3PWM);
      motor4.writeMicroseconds(Motor4PWM);
     } else {
      motor1.writeMicroseconds(1000);
      motor2.writeMicroseconds(1000);
      motor3.writeMicroseconds(1000);
      motor4.writeMicroseconds(1000);
      perrorIntegral = 0;
      qerrorIntegral = 0;
      rerrorIntegral = 0;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
    if ((millis() - LastCmdTime) >= CmdPeriod) //reading of the receiver.
    {
         LastCmdTime = millis();
         readReceiver();  
    }
    
    if ((millis() - LastOutTime) >= OutPeriod) //reading of the motors.
    {
	 readIMU();
	 float elapsedTime = millis() - LastOutTime;
	 callStablizer(elapsedTime);
         callMotors();
	 LastOutTime = millis();
    }
}
