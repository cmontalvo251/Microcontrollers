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
//#define RESISTORPINB 6
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
//imu::Vector<3> lin_accel;

//Arduino Time
float lastPrint = 0;

//GPS//
#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial1);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

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
float psiH = -99;
//float psi_command = 0;
  
//SD//
//#include <SPI.h> // what is this?
#include <SD.h>
File myFile;

///////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // SD //
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (53 on the Mega) must be left as an output or the SD library functions will not work. 
  pinMode(53, OUTPUT);
  
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while(1){};
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char filename[15];
  strcpy(filename, "QUAD000.TXT");
  for (uint8_t i = 0; i < 1000; i++) {
    filename[4] = '0' + i/100;
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  char lastfile[15];
  strcpy(lastfile,"QUAD999.TXT");
  if (SD.exists(lastfile)){
    Serial.print("Sorry QUAD_##.TXT has reached its limit. Suggest wiping SD card");
    while(1){};
  }
  myFile = SD.open(filename, FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (!myFile) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while(1){};
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
  
  
  // Attach ESC ("Servos")
  upper_left.attach(UPPERLEFT);
  lower_right.attach(LOWERRIGHT);
  lower_left.attach(LOWERLEFT);
  upper_right.attach(UPPERRIGHT);

   //Setup RESISTORPINS as output to write digital HIGH or LOW to switch transistor array on or off.
   pinMode(RESISTORPIN,OUTPUT);
   //pinMode(RESISTORPINB,OUTPUT);
  
  // Reading from pin 11 for Trainer Switch
  //Set up PORTB to interuppt code when it gets a reading.
  //PCICR |= (1 << PCIE0); //PORTB 0-7 // not sure what this is - PORTB is an 8 bit register that contains pins 50-53 and 10-13. 
  //This would be different if we used PORTC or PORTD which correspond to different pins
  //PCMSK0 |= (1 << PCINT4); //PORTB4 - Digital Pin 10
  //PCMSK0 |= (1 << PCINT5); //PORTB5 - Digital Pin 11  // why ('|=') This symbol is a bit wise or. It says use what is currently in PCMSK0 or the result of 1 << PCINT5
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
 
 
  // GPS //
  GPS.begin(9600);
  
  Serial.print("GPS Class Initialized \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //turn on RMC(recommended minimum)
  Serial.print("NMEA OUTPUT Set \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  // 1 or 10 Hz update rate. what should i use?? Let's try 10 Hz. If things stop working we can set to 1Hz. I bet it just draws more current when it updates at 10Hz
  //But I might be wrong. It might actually send data serially more often at which point our code will bog down. Again this is a pretty simple change so we can cross that bridge later.
  
  Serial.print("Update Rate Set \n");
  
  useInterrupt(false);
  
  Serial.print("Delaying for  1 second. Don't move anything \n");
  delay(1000);
  
  lastPrint = millis()/1000.0;
}


/////////////////////////////////////////////////////////////////////////////////////
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMERO_COMPA_vect) {
  char c = GPS.read();
  if (GPSECHO)
    if (c) UDR0 = c; // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}
///////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
  // GPS //
  if (! usingInterrupt) {
     // read data from the GPS in the 'main loop'
     char c = GPS.read();
     // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
      //if (c) UDR0 = c; //writing direct to UDR0 is much faster than Serial.print
      //but only one character can be written at a time.
  }
 
 //if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
    return;
  }
  
  //Check on Trainer Switch
  autopilot = 0;

  ///USING META 1 Dx5e - CH5 routed to GEAR on receiver and hardwired to pin 11 using Register MASKS
  // CH5 = 0 - GEAR = 1900 ms
  // CH5 = 1 - GEAR = 1600 ms - Rotors ARM
  // CH5 = 2 - GEAR = 1100 ms - Rotors ARM - Need to set this to autopilot as well.
  pulse_timeTrainer = pulseIn(11,HIGH);
  if (pulse_timeTrainer < 500) {
        pulse_timeTrainer = 500;
  }
  if (pulse_timeTrainer > 2400) {
     pulse_timeTrainer = 2400;
  }
  if ((pulse_timeTrainer > 1900) and (pulse_timeTrainer < 1920) and (QUADON == 0)) {
     QUADON = 1;
  }
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
    //digitalWrite(RESISTORPINB,HIGH);
    
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
    float kproll = 0.8; //0.5 or 3 //set to zero for now but I bet it's something like kp = 40 and kd = 15
    float kdroll = 35; //25 //Need to check cleanflight because I bet the units are the same. 
    float kppitch = 0.8; //0.5 or 3 //If kp = 40 and roll angle = 45 degress - 40*45 = 1800 microseconds. Yeesh. 
    float kdpitch = 35; //25 //Ok maybe the units are different. What if it was kp = 40*pi/180? 40*45*pi/180 = 31 microseconds?
    float kppsi = 2; 
    float kdpsi = 1;

    ///DEBUGGING ONLY!!!!
    int nominal_val = 1200;
    int min_val = 1100;
    kproll = 10;
    kdroll = 0;
    kppitch = 10;
    kdpitch = 0;
    kppsi = 0;
    kdpsi = 0;
    //////////////////////
    
    //ok so 1800 seems huge and 31 seems too low. Not sure what to do. I think I just need to shoot for like 200 microseconds at 15 degrees.
    // so 200 = kp*15 so kp = 13.33 so let's set kp = 15 and kd = 5 to start and see if it works. <- Definitely not
    float roll_command = kproll*roll + kdroll*roll_rate;
    float pitch_command = kppitch*pitch + kdpitch*pitch_rate;
    float heading_command = kppsi*(yaw-psiH) + kdpsi*yaw_rate;

    //At a value of 1560 we think this will just put the quad into autoland while holding attitude.

    
    
    //int nominal_val = 1525; //1560 //We tested this using cleanflight and it seems like that is correct.
    //int min_val = 1400; //1400 //Minimum value to make sure I don't fall out of the sky
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
    //digitalWrite(RESISTORPINB,LOW);
  }


  //printing if clause
  if (millis()/1000.0 > 0.1 + lastPrint) {
    
    lastPrint = millis()/1000.0; // reset timer

    // PITOT //
    //int pitotValue = analogRead(PITOTPIN); //Pitot in analog 9 - I think Rockwell put the Pitot in A9 not A14 on the PCB. Will have to check
   
    // does this need to be in this if clause? Yes it does. Otherwise we poll the attitude vector everysingle loop.
    //Not quite sure if this is ok. We may need to sample Euler angles every loop or put an if statement and poll euler when
    //autopilot is on. That way we can generate our error signals. It might be ok to just do it here. Maybe we should make euler a global?
    if (autopilot == 0) {
      euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
      rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    }
    //lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL); //Not sure if we really need this either
    //int8_t temp = bno.getTemp(); //We may not need this. The iMet sensor will have temp. Not to mention this will output temp of the inside of the airplane which will be considerably hotter 
    //So this is probably just garbage data anyway.
  
    //SERIAL PRINTS
    //GPS Sensor
    Serial.print(GPS.year); Serial.print(" ");
    Serial.print(GPS.month); Serial.print(" ");
    Serial.print(GPS.day); Serial.print(" ");
    Serial.print(GPS.hour, DEC); Serial.print(' ');
    Serial.print(GPS.minute, DEC); Serial.print(' ');
    Serial.print(GPS.seconds, DEC); Serial.print(' ');
    Serial.print(lastPrint); Serial.print(" ");
    Serial.print((int)GPS.fix); Serial.print(" ");
    Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
    Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
    Serial.print(GPS.speed); Serial.print(" ");
    Serial.print(GPS.angle); Serial.print(" ");
    Serial.print(GPS.altitude); Serial.print(" ");

    //IMU Data
    Serial.print(euler.x()); Serial.print(" "); //This is Positive yaw (Degrees)
    Serial.print(euler.y()); Serial.print(" "); //This is positive pitch (Degrees)
    Serial.print(euler.z()); Serial.print(" "); //This is negative roll (Degrees)
    Serial.print(rate.x()); Serial.print(" "); //This is positive roll rate (rad/s)
    Serial.print(rate.y()); Serial.print(" "); //This is negative pitch rate (rad/s)
    Serial.print(rate.z()); Serial.print(" "); //This is negative yaw rate (rad/s)
    //Serial.print(lin_accel.x()); Serial.print(" ");
    //Serial.print(lin_accel.y()); Serial.print(" ");
    //Serial.print(lin_accel.z()); Serial.print(" ");
    //Serial.print(temp); Serial.print(" ");
   
    //PITOT SENSOR
    //Serial.print(pitotValue); Serial.print(" ");
   
    //Servos and autopilot health
    Serial.print(pulse_timeTrainer); Serial.print(" ");
    Serial.print(autopilot); Serial.print(" ");
    Serial.print(QUADON); Serial.print(" ");
    Serial.print(upper_left_command); Serial.print(" ");
    Serial.print(lower_left_command); Serial.print(" ");
    Serial.print(upper_right_command); Serial.print(" ");
    Serial.print(lower_right_command); Serial.print(" ");
    Serial.print(psiH); Serial.print(" ");
    Serial.print("\n");
  
   /* Write data to file */
   //GPS Sensor
   myFile.print(GPS.year); myFile.print(" ");
   myFile.print(GPS.month); myFile.print(" ");
   myFile.print(GPS.day); myFile.print(" ");
   myFile.print(GPS.hour, DEC); myFile.print(' ');
   myFile.print(GPS.minute, DEC); myFile.print(' ');
   myFile.print(GPS.seconds, DEC); myFile.print(' ');
   myFile.print(lastPrint); myFile.print(" ");
   myFile.print((int)GPS.fix); myFile.print(" ");
   myFile.print(GPS.latitudeDegrees, 8); myFile.print(" ");
   myFile.print(GPS.longitudeDegrees, 8); myFile.print(" "); 
   myFile.print(GPS.speed); myFile.print(" ");
   myFile.print(GPS.angle); myFile.print(" ");
   myFile.print(GPS.altitude); myFile.print(" ");
  
   //IMU Data
   myFile.print(euler.x()); myFile.print(" ");
   myFile.print(euler.y()); myFile.print(" ");
   myFile.print(euler.z()); myFile.print(" ");
   myFile.print(rate.x()); myFile.print(" ");
   myFile.print(rate.y()); myFile.print(" ");
   myFile.print(rate.z()); myFile.print(" ");
   //myFile.print(lin_accel.x()); myFile.print(" ");
   //myFile.print(lin_accel.y()); myFile.print(" ");
   //myFile.print(lin_accel.z()); myFile.print(" ");
   //myFile.print(temp); myFile.print(" "); ---- So we may actually need this since temperature effects the pitot sensor.
   //not to mention we may actually need pressure as well. Not sure what to do about that. The 10DOF sensor would have been the way to go
   
   //PITOT
   //myFile.print(pitotValue); myFile.print(" ");
   
   //Servos and autopilot health
   myFile.print(pulse_timeTrainer); myFile.print(" ");
   myFile.print(autopilot); myFile.print(" ");
   myFile.print(QUADON); myFile.print(" ");
   myFile.print(upper_left_command); myFile.print(" ");
   myFile.print(lower_left_command); myFile.print(" ");
   myFile.print(upper_right_command); myFile.print(" ");
   myFile.print(lower_right_command); myFile.print(" ");
   myFile.print(psiH); myFile.print(" ");
   myFile.print("\n");
   
   //Flush the file
   myFile.flush();
  }
}


//////////////////////////////////////////////////////////////////////////////////////
//ISR(PCINT0_vect) { //for PORTB0-7

  //current_time = micros();

  //This is archived code from when we initially were going to read 
  //Servo signals directly. Now that we have moved to a trainer switch 
  //and are using the transistor array we have no need to measure elevator and aileron.

  //Reading elevator from pin 12
//  if (PINB & B01000000) { 
//  
//    if(last_channelE == 0)
//    {
//      last_channelE = 1;
//      timerE = current_time;
//    }
//  }
//  else if(last_channelE == 1){
//      last_channelE = 0;
//      pulse_timeE = current_time - timerE;
//  }
//
// //Reading aileron from pin 11
// if (PINB & B00100000) { 
//  
//    if(last_channelA == 0)
//    {
//      last_channelA = 1;
//      timerA = current_time;
//    }
//  }
//  else if(last_channelA == 1){
//      last_channelA = 0;
//      pulse_timeA = current_time - timerA;
//  }

  //We are now reading the trainer switch from pin 11
  //if (PINB & B00100000) { //PORTB5 is Digital Pin 11 MEGA
  //  if (last_channelTrainer == 0) {
  //    last_channelTrainer = 1;
  //    timerTrainer = current_time;
  //  }
  //}
  //else if (last_channelTrainer == 1) {
  //    last_channelTrainer = 0;
  //    pulse_timeTrainer = current_time - timerTrainer;
  //    if (pulse_timeTrainer < 500) {
  //      pulse_timeTrainer = 500;
  //    }
  //    if (pulse_timeTrainer > 2400) {
   //     pulse_timeTrainer = 2400;
   //   }
  //    if ((pulse_timeTrainer > 1900) and (pulse_timeTrainer < 1920) and (QUADON == 0)) {
  //      QUADON = 1;
  //    }
  //}

//}
