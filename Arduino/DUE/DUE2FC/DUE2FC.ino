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

//FORKED CODE TO DUE
//Rev 5.0 - Code compiles. Code flashed and can read the IMU and BMP sensor
//Rev 5.1 - Added receiver.h/cpp from KyrellGod to read Receiver.
//Rev 5.2 - Added switch case for autopilot logic.
//Rev 5.3 - Added servo library to send commands to Naze. I don't think it needs to be that fast for inner loop outer loop control
//Rev 5.4 - Flight tested with Arduino DUE - 2/16/2018
//Rev 5.5 - Added some debug prints because of sluggish flying qualities.
//Rev 5.6 - Added code for data logging 
//Rev 5.7 - Code has started to lag significanlty. Currenlty adding steps to make it faster. Added looptimers and a scheduler.
//Rev 5.8 - Flight tested successfully and data plotted - 2/22/2019 - End rev 5

//ALTITUDE HOLD MODE
//Rev 6.0 - Outputted pressure and got rid of GEAR commands from data file. Going to flight test with just outputting pressure and Az to see what the data looks like. Flight test successful on 2/24/2019
//Rev 6.1 - Changed I2C/datalogging rate to 10 Hz. No longer need to measure Az. Also added a low pass filter to data
//Rev 6.2 - Added the altitude controller and am just tuning gains now. Below is a list of flight tests
// 3/9/2019 - Delta5 FPV field - KP = 100, Ts = 8.0
// 3/12/2019 - Intramural Fields - KP = 25, Ts = 6.0 added a constrain function to +-250 -- Unstable oscillations occured. 
//Need to decrease KP to 10 and lower settling time to 4.0 May need to add integral gain.
// 3/20/2019 - In my front yard - KP = 10, Ts = 4.0 - Oscillations still occured but they were slower. Suggest lowering Kp to 5 and adding integral gain
// 3/20/2019 -   "              - KP = 5, Ts = 4.0, KI = 0.001 - No oscillations but I think it just stayed in ground effect. Suggest increase Kp and Ki
// 3/20/2019 -  "               - KP = 8, Ts = 4.0, KI = 0.01 - Oscillations returned - Maybe Kp back to 5?
// KP 5, TS 4, KI = 0.005 - Ground effect flying returned so suggest KP = 6.5 and leave everything else the same. Also battery died after 4 flights so I suggest buying a bigger battery
// Rev 6.3 - Added an altitude failsafe in the event altitude is over 200 meters. No way I'm flying that high with this thing
//I feel like 4.0 seconds is just way too slow. I'm setting it to 2.0 seconds. In reality the integral gain should settle that out.
//KP 6.5, KI 0.005, TS 2.0 - 3/30/2019 - Flight was really good. No oscillations. It had quite a bit of steady state error so may increase Ki?
//KP 6.5, KI 0.01, TS 2.0 - same day - Flight was pretty good. More oscillations but I think it was just wind to be honest. I bet it could do better out of winds.
//IMO - Move on to Rev 7.

//FPV and GPS
//Rev 7.0 - Added an FPV camera so no change to code but it's easier to fly long distance and make sure the drone is doing what it's supposed 
//to be doing
//Rev 7.1 - Added a GPS and code to read it. GPS definitely gets a lock indoors but when I add it to this software I cannot get a lock. Gauranteed there is an issue with reading a char.
//Either we need to turn interrupts on or we need to turn some non-essential things off so that we can get a lock.
//Rev 7.2 - Moved everything into their own cpp routines 
//Rev 7.3 - Added a scheduler to make sure everything has enough time to run.

//Telemetry
//Rev 8.0 - Bare bones telemetry added to send pitch, roll yaw to ground station.
//Rev 8.1 - Telemetry code altered to send altitude information instead.

#include "receiver.h"
#include "userparams.h"
#include "mathHelp.h"
#include "i2c.h"
#include "gps.h"
#include "sd.h"
#include "commands.h"
#include "telemetry.h"

receiver RX;
sdcard mysd;
Montalvo_GPS myGPS;
i2c myi2c;
commands FC;
Telemetry Xbee;

float loopTimer = 0;
float lastSerialTime = 0;
float lastPrintToFileTime = 0;

//Serial print routine
void cout_endl() {  
  #ifdef DEBUGPRINTS
  Serial.print("Printing to STD OUT. Time = ");
  Serial.print(loopTimer);
  Serial.print(" Elapsed Time = ");
  Serial.println(loopTimer - lastSerialTime,6);
  #endif
  
  lastSerialTime = millis()/1000.0;

  Serial.print(loopTimer);
  Serial.print(" ");

  RX.print_RX();
  FC.print_Commands();
  myi2c.print_I2C();
  myGPS.print_GPS();

  Serial.print("\n");
}

//Print to File routine
void print_to_file() {
  #ifdef DEBUGPRINTS
  Serial.print("Printing To File. Time = ");
  Serial.print(loopTimer);
  Serial.print(" Elapsed Time = ");
  Serial.println(loopTimer - lastPrintToFileTime,6);
  Serial.println("SD");
  #endif
  lastPrintToFileTime = millis()/1000.0;
  
  mysd.myFile.print(loopTimer);
  mysd.myFile.print(" ");
  mysd.myFile.print(RX.MODE);
  mysd.myFile.print(" ");
  
  RX.print_SD_receiver(mysd.myFile);
  FC.print_SD_Commands(mysd.myFile);
  myi2c.read_I2C(loopTimer);
  myi2c.print_SD_I2C(mysd.myFile);
  myGPS.print_SD_GPS(mysd.myFile);
  
  mysd.myFile.print("\n");
  mysd.myFile.flush();
}

//Setup routines
void setup(){

  #if PRINTSERIAL
    Serial.begin(115200);
  #endif

  RX.setup_RX();
  mysd.setup_SD();
  myGPS.setup_GPS();
  myi2c.setup_I2C();
  FC.setup_servo();
  Xbee.begin(&Serial2);
  
}


//Main program loop
void loop(){

  //Get New loop time
  loopTimer = millis()/1000.0;

  myGPS.poll_GPS(); //Unfortunately since we aren't using interrupts we need to poll
  //GPS all the time otherwise we will lose information. Hopefully this doesn't
  //totally fuck everything up. So it did fuck everything up but we've slowed the rest of the simulation
  //down to get to a point where the GPS works. We also added a while Serial.available routine in the GPS block
  //to keep reading and we don't lose any information. 

  ///Get Euler Angles and Pressure Data
  //This only needs to run when writing to the SD card.
  //if ((loopTimer-lastI2CTime) > I2C_PERIOD) {
  //  read_I2C();
  //}

  //Poll Receiver
  if ((loopTimer - RX.lastReceiverTime) > RECEIVER_PERIOD) {
    RX.poll_receiver(myi2c);
  }

  //Autopilot switch case -- We want this to run as fast as possible right?
  //Maybe not? I mean the innerloop must be fast but maybe not this one?
  if ((loopTimer - FC.lastComputeControlTime) > CONTROL_PERIOD) {
    FC.compute_Control(RX,myi2c); //Ok actually compute_Control should just happen right after poll receiver?
    //Wait wait wait. If you're running autopilot you might want this on and running in the background
  }

  //Send Commands to NAZE
  //Same with this? Does this need to be that fast?
  if ((loopTimer - FC.lastSendCommandsTime) > COMMAND_PERIOD) {
    FC.send_Commands(loopTimer);
  }

  //Print to File
  //digitalWrite(13,HIGH);
  if ((loopTimer - lastPrintToFileTime) > FILEPRINT_PERIOD) {
    //digitalWrite(13,LOW);
    print_to_file();
  }

  ///Send Data over Xbee
  if ((loopTimer - Xbee.last_read) > XBEE_PERIOD) {
    Xbee.serialoutarray[0] = loopTimer+XBEE_PERIOD; //This is to remove delay in the send command
    //Set the number of serial out array in userparams.h
    //Xbee.serialoutarray[1] = myi2c.roll;
    //Xbee.serialoutarray[2] = myi2c.pitch;
    //Xbee.serialoutarray[3] = myi2c.yaw;
    Xbee.serialoutarray[1] = myi2c.altitude_raw_new;
    Xbee.write(loopTimer);
  }
  
  //Print Controller Health
  #if PRINTSERIAL
  if ((loopTimer - lastSerialTime) > SERIALPRINT_PERIOD) {
    cout_endl();
  }
  #endif
  
}
