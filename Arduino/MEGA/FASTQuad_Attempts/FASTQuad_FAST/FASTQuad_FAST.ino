// FAST Quad sensor package includes 9DOF

// OK SO ALOT OF THIS CODE IS FROM FASTWING SO WE NEED TO CHANGE SOME THINGS. FOR ONE WE HAVE TWO RESISTORPINS OR HIGHLOWPINS
// THE FIRST IS ON PIN 5 and the other is on Pin 6
// As per Dr. Thomas' recommendation I have changed this to be only 1 pin - pin 5
// I used cleanflight and found out that anything over 1100 ms will spin up the motors so I just need a way to go from
// ms to degrees.

// When you are programming your autopilot you need to convert ms to degrees. This is what I have
// 0 deg = 552 ms
// 180 deg = 2400 ms
// Let me google it as well. Ok turns out I was right but guess what? There's a function called writeMicroseconds. So
// just use that function instead.

// The GEAR pin right now is taken from the receiver and split into two signals. The first goes to the arduino and the second goes
// to the flight controller. The arduino reads this GEAR signal and is currently hardwired to pin 11. You'll need to use the fancy
// read pin stuff to get that to work. GEAR on the Dx5e is wired to CH5. 
// CH5 = 0 - GEAR = 1900 ms
// CH5 = 1 - GEAR = 1600 ms - Rotors ARM
// CH5 = 2 - GEAR = 1100 ms - Rotors ARM - Need to set this to autopilot as well.


///PIN LAYOUT
#define RESISTORPIN 5
#define UPPERLEFT 8
#define LOWERLEFT 7
#define LOWERRIGHT 9
#define UPPERRIGHT 10

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//IMU Data
imu::Vector<3> euler;
imu::Vector<3> rate;

//Arduino Time
float lastPrint = 0;
float psiH = -99;

//Servo//
#include <Servo.h>
Servo upper_left,upper_right,lower_left,lower_right;
unsigned long current_time,timerTrainer; 
byte last_channelTrainer;
int pulse_timeTrainer;
int autopilot;
int QUADON = 0;

int upper_left_command = 1200; //milliseconds
int upper_right_command = 1200;
int lower_left_command = 1200;
int lower_right_command = 1200;
  
///////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(115200);

  // Attach ESC ("Servos")
  upper_left.attach(UPPERLEFT);
  lower_right.attach(LOWERRIGHT);
  lower_left.attach(LOWERLEFT);
  upper_right.attach(UPPERRIGHT);

   //Setup RESISTORPINS as output to write digital HIGH or LOW to switch transistor array on or off.
   pinMode(RESISTORPIN,OUTPUT);
  
  // Reading from pin 11 for Trainer Switch
  //Set up PORTB to interuppt code when it gets a reading.
  PCICR |= (1 << PCIE0); //PORTB 0-7 // not sure what this is - PORTB is an 8 bit register that contains pins 50-53 and 10-13. 
  //This would be different if we used PORTC or PORTD which correspond to different pins
  //PCMSK0 |= (1 << PCINT4); //PORTB4 - Digital Pin 10
  PCMSK0 |= (1 << PCINT5); //PORTB5 - Digital Pin 11  // why ('|=') This symbol is a bit wise or. It says use what is currently in PCMSK0 or the result of 1 << PCINT5
  //PCMSK0 |= (1 << PCINT6); //PORTB6 - Digital Pin 12 // PCINT5 is an 8 bit number. 1 << PCINT5 means take 1 and bit shift it by the amount in PCINT5
  
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
 
  Serial.print("Delaying for  1 second. Don't move anything \n");
  delay(1000);

  lastPrint = millis()/1000.0;

  Serial.print("Setup Complete \n");
  
}

void loop() {
    
  //Check on Trainer Switch
  autopilot = 0;

  ///USING META 1 Dx5e - CH5 routed to GEAR on receiver and hardwired to pin 11 using Register MASKS
  // CH5 = 0 - GEAR = 1900 ms
  // CH5 = 1 - GEAR = 1600 ms - Rotors ARM
  // CH5 = 2 - GEAR = 1100 ms - Rotors ARM - Need to set this to autopilot as well.
  if ((pulse_timeTrainer < 1400) and (QUADON)) {
    autopilot = 1;
  }
  if ((pulse_timeTrainer > 1400) and (QUADON)) {
    psiH = -99;
  }
 
  ////////USING Carlos' Dx6 and reading the Rudder Signal
  
//  psi_command = -99;
//  if (pulse_timeTrainer > 1600) {
//    autopilot = 1;
//    psi_command = 0;
//  }
//  if (pulse_timeTrainer < 1300) {
//    autopilot = 2;
//    psi_command = 180;
//  }
//  if (pulse_timeTrainer == 0) {
//    autopilot = 0;
//    psi_command = -99;
//  }

  ///////USING Dx7 and reading 2-pin switch
  
  //if (pulse_timeTrainer < 1500) {
    //if (autopilot == 0) {
    //  if (psi_command == 0) {
    //    psi_command = 180;
    //  }
    //  else
    //  {
    //psi_command = 0;
    //  }
   // }
    //autopilot = 1;
  //}
  //else {
  //  autopilot = 0;
  //}
  //if (pulse_timeTrainer == 0){
  // autopilot = 0;
  //}

  // Default is to send a LOW signal to the Digital Write HIGH or LOW flag - D11
  // If Trainer switch is HIGH we send HIGH to D5 and D6 if Trainer Switch is LOW we send a LOW signal to D5 and D6
  if (autopilot) {
    digitalWrite(RESISTORPIN,HIGH); //Using pin D5 to output high or low.
    
    //Attitude Hold Algorithm
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

    //Extract 3-2-1 standard convention
    float roll = -euler.z();
    float pitch = euler.y();
    float yaw = euler.x(); //These are all in degrees

    if (yaw > 180.0) {
      yaw -= 360;
    }
    if (psiH == -99) {
      psiH = yaw;
    }

    float roll_rate = rate.x(); //These are all in rad/s....I think.
    float pitch_rate = -rate.y();
    float yaw_rate = -rate.z();
    
    //PD controller for commands
    //CleanFlight defaults are 45,30 for kp,kd
    //DO NOT MAKE KP BIGGER THAN 5!!!!!!
    //I SUGGEST DOING KP = 0.5 and KD = 25
    //I tried kp = 10 and Kd = 60 and that broke the quad.
    //I tried kp = 3 and kd = 25 and that was pretty twitchy

    ///Tested in the lab - KP = 1 is good. 5/12/2017
    //DO NOT CHANGE KP LEAVE AT 1.0, can increase KD if you want but go slow.
    float kproll = 1; //0.5 or 3 //set to zero for now but I bet it's something like kp = 40 and kd = 15
    float kdroll = 35; //25 //Need to check cleanflight because I bet the units are the same. 
    float kppitch = 1; //0.5 or 3 //If kp = 40 and roll angle = 45 degress - 40*45 = 1800 microseconds. Yeesh. 
    float kdpitch = 35; //25 //Ok maybe the units are different. What if it was kp = 40*pi/180? 40*45*pi/180 = 31 microseconds?
    float kppsi = 2; 
    float kdpsi = 1;
    
    //ok so 1800 seems huge and 31 seems too low. Not sure what to do. I think I just need to shoot for like 200 microseconds at 15 degrees.
    // so 200 = kp*15 so kp = 13.33 so let's set kp = 15 and kd = 5 to start and see if it works. <- Definitely not
    float roll_command = kproll*roll + kdroll*roll_rate;
    float pitch_command = kppitch*pitch + kdpitch*pitch_rate;
    float heading_command = kppsi*(yaw-psiH) + kdpsi*yaw_rate;

    //At a value of 1560 we think this will just put the quad into autoland while holding attitude.
    int nominal_val = 1560; //1560 //We tested this using cleanflight and it seems like that is correct.
    //We did some more tests an we think 1500 is better
    int min_val = 1400; //1400 //Minimum value to make sure I don't fall out of the sky
    int max_val = 1900; //1900 //I commanded 1900 in clean flight and it seems ok so 1800 I think is safe but maybe it needs to be bigger?

    //For debugging
    //int nominal_val = 1200;
    //int min_val = 1000;
    
    //Send these commands to each micro signal
    upper_left_command = nominal_val - roll_command - pitch_command + heading_command; //I think I have the signs right - 5/5/2017
    upper_right_command = nominal_val + roll_command - pitch_command - heading_command;
    lower_left_command = nominal_val - roll_command + pitch_command - heading_command;
    lower_right_command = nominal_val + roll_command + pitch_command + heading_command;

    //Some checks
    if (upper_left_command < min_val) {
      upper_left_command = min_val;
    }
    if (upper_right_command < min_val) {
      upper_right_command = min_val;
    }
    if (lower_left_command < min_val) {
      lower_left_command = min_val;
    }
    if (lower_right_command < min_val) {
      lower_right_command = min_val;
    }

    //Make sure we don't burn up the motors and set a saturation limiter
    if (upper_left_command > max_val) {
      upper_left_command = max_val;
    }
    if (upper_right_command > max_val) {
      upper_right_command = max_val;
    }
    if (lower_left_command > max_val) {
      lower_left_command = max_val;
    }
    if (lower_right_command > max_val) {
      lower_right_command = max_val;
    }
    
    //Write commands to ESCs
    upper_left.writeMicroseconds(upper_left_command);
    upper_right.writeMicroseconds(upper_right_command);
    lower_left.writeMicroseconds(lower_left_command);
    lower_right.writeMicroseconds(lower_right_command);
  }
  else {
    digitalWrite(RESISTORPIN,LOW);
  }

  //if (millis()/1000.0 > 0.05 + lastPrint) {
  //  lastPrint = millis()/1000.0; // reset timer
  //  Serial.print(lastPrint);    Serial.print(" ");
  //  Serial.print(pulse_timeTrainer);Serial.print(" ");
  //  Serial.print(upper_left_command);    Serial.print(" ");
  //  Serial.print(upper_right_command);    Serial.print(" ");
  //  Serial.print(lower_left_command);    Serial.print(" ");
  //  Serial.print(lower_right_command);    Serial.print(" ");
  //  Serial.print("\n");
  //}
  
  
}


//////////////////////////////////////////////////////////////////////////////////////
ISR(PCINT0_vect) { //for PORTB0-7

  current_time = micros();
  
  //We are now reading the trainer switch from pin 11
  if (PINB & B00100000) { //PORTB5 is Digital Pin 11 MEGA
    if (last_channelTrainer == 0) {
      last_channelTrainer = 1;
      timerTrainer = current_time;
    }
  }
  else if (last_channelTrainer == 1) {
      last_channelTrainer = 0;
      pulse_timeTrainer = current_time - timerTrainer;
      if (pulse_timeTrainer < 500) {
        pulse_timeTrainer = 500;
      }
      if (pulse_timeTrainer > 2400) {
        pulse_timeTrainer = 2400;
      }
      if ((pulse_timeTrainer > 1900) and (pulse_timeTrainer < 1920) and (QUADON == 0)) {
        QUADON = 1;
      }
  }

}
