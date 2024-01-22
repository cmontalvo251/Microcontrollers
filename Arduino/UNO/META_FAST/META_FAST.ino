// Meta Aircraft sensor package includes GPS, 9DOF, pitot probe, including capabilities to write 
// to two servos through a transistor array using pin 10 to read a trainer switch from a receiver
// pin 7 to switch between manual and autopilot and pins 8 and 9 to write to two servos.

///PIN LAYOUT
//ANALOG
#define PITOTPIN 9 //Analog

//DIGITAL
#define HIGHLOWPIN 7 //Digital
#define AILERONPIN 8   //Digital
#define ELEVATORPIN 9  //Digital
//D10 is bitwise hardcoded to read the trainer switch

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// vector_type field can be one of the following values:
// VECTOR_MAGNETOMETER (values in uT, micro Teslas)
// VECTOR_GYROSCOPE (values in rps, radians per second)
// VECTOR_EULER (values in Euler angles or 'degrees', from 0..359)
// VECTOR_ACCELEROMETER (values in m/s^2)
// VECTOR_LINEARACCEL (values in m/s^2)
// VECTOR_GRAVITY (values in m/s^2)
// https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/wiring-and-test

// NOTICE THAT VECTOR_GYROSCOPE is in rad/s
// BUT VECTOR_EULER is in degrees

//IMU Data
imu::Vector<3> euler;
imu::Vector<3> rate;

//WAYPOINT CONTROL #DEFINES
#define BUBBLE 8
//This 8 is our bubble. We may want to make it bigger.
//Intramural Fields
//#define Ox 30.695691
//#define Oy -88.192942
//Municipal Park
#define OriginX 30.706606
#define OriginY -88.159402
#define OFFSET 0.0
#define TRACK_DISTANCE 100

//Autopilot
//Might need to change this so we don't climb so much
#define pitchc 0 
#define kp_pitch 0.6
#define kd_pitch 1.5
#define pitch_ratec 0
//Might want to do 90 or 180 one day but that's once you can actually command a heading no problem.
//#define psi_command 0
#define kp_roll 0.8
#define kd_roll 2.0 
#define roll_ratec 0 

//Compute which waypoint we need to travel to
float WPx[4],WPy[4]; 
int WPcounter = 0;
float x=0,y=0,dpsi,xcommand,ycommand;

//Arduino Time
float lastPrint = 0;

//GPS//
#include <FASTGPS.h>
Adafruit_GPS GPS(&Serial1);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false; //Without interrupts the Arduino is not fast enough to get GPS while autopilot is on.
//Jitter as well as this issue is probably because of the speed of reading the euler angles. I suggest moving euler angles to a separate clock
//cycle. Have a clock cycle for writing to SD and another for reading euler angles. That way you don't run into the euler angle issue you had on 1/16/18
//where failsafe mode did not write euler angles.
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//Servo//
#include <Servo.h>
Servo aileron,elevator; //aileron, elevator, throttle servos.. just reading for now
unsigned long current_time,timerTrainer;
byte last_channelTrainer;
int pulse_timeTrainer;
int autopilot, backup_receiver;
int aileronCommand=90;
int elevatorCommand=90;
float psi_command = 0, heading = 0,psi_GPS_prev=-99,psi_IMU_prev=-99;
//float autoOn = -99;

#define PRINTSERIAL 0
//#define META 1 FAILSAFE REMOVED AS PER FLIGHT TEST 1/16/18
  
//SD//
//#include <SPI.h> // what is this?
#include <SD.h>
File myFile;

///////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  //Setup waypoints
  WPx[0] = TRACK_DISTANCE;    WPy[0] = 0.0+OFFSET; 
  WPx[1] = 0.0;               WPy[1] = TRACK_DISTANCE+OFFSET;
  WPx[2] = TRACK_DISTANCE;    WPy[2] = TRACK_DISTANCE+OFFSET;
  WPx[3] = 0.0;               WPy[3] = 0.0+OFFSET;

  xcommand = WPx[0];
  ycommand = WPy[0];
  
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
  strcpy(filename, "META000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[4] = '0' + i/100;
    filename[5] = '0' + (i/10)%10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  char lastfile[15];
  strcpy(lastfile,"META999.TXT");
  if (SD.exists(lastfile)){
    Serial.print("Sorry SD card has reached its naming limit. Suggest wiping SD card");
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
  
  
  // Attach Servos //
  // Writting to pins 8 9 and 10 (throttle if we ever need it)
  aileron.attach(AILERONPIN);
  elevator.attach(ELEVATORPIN);

   //Setup 7 as output to write digital HIGH or LOW to switch transistor array on or off.
   pinMode(HIGHLOWPIN,OUTPUT);
  
  // Reading from pin 11 for Trainer Switch
  //Set up PORTB to interuppt code when it gets a reading.
  PCICR |= (1 << PCIE0); //PORTB 0-7 // not sure what this is - PORTB is an 8 bit register that contains pins 50-53 and 10-13. 
  //This would be different if we used PORTC or PORTD which correspond to different pins
  PCMSK0 |= (1 << PCINT4); //PORTB4 - Digital Pin 10
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
  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);  // 1 or 10 Hz update rate. what should i use?? Let's try 10 Hz. If things stop working we can set to 1Hz. I bet it just draws more current when it updates at 10Hz
  //But I might be wrong. It might actually send data serially more often at which point our code will bog down. Again this is a pretty simple change so we can cross that bridge later.

  //Set Origin Latitude and Longitude
  GPS.LatOrigin = OriginX;
  GPS.LonOrigin = OriginY;
  
  Serial.print("Update Rate Set \n");
  
  useInterrupt(false);
  
  Serial.print("Delaying for  1 second. Don't move anything \n");
  delay(1000);

  //NEED TO FIGURE OUT euler.xyz and rate.xyz on the device and the units
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  UpdateHeading(euler.x(),GPS.angle); //this is our new UpdateHeading control law
  
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
    //if you want to debug, this is a good time to do it!
    if (GPSECHO)
      //if (c) Serial.print(c);
      if (c) UDR0 = c; //writing direct to UDR0 is much faster than Serial.print
      //but only one character can be written at a time.
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    GPS.parse(GPS.lastNMEA());
  }
  
  //Check on Trainer Switch
  //Set Defaults if pulse_trainer doesn't fall into one of these buckets - SUGGEST MOVING THIS 3-PIN Switch to a 2-PIN (GEAR1)
  autopilot = 0;
  //if (pulse_timeTrainer > 1550) {
  // autoOn = -99; 
  //}
  if ((pulse_timeTrainer < 1550) and (pulse_timeTrainer > 1450)) {
    //Switch is in position 1 - Autopilot mode
    autopilot = 1;
  }
  if ((pulse_timeTrainer < 1200) and (pulse_timeTrainer > 1000)) {
    autopilot = 1;
    //if (META == 2) {
    //  autopilot = 3;
    //}
    //if (autoOn == -99) {
    //  autoOn = millis()/1000.0; //USED FOR DOUBLET MANUEVERS
    //}
  }
  
  // Default is to send a LOW signal to the Digital Write HIGH or LOW flag - D7
  // If Trainer switch is HIGH we send HIGH to D11 if Trainer Switch is LOW we send a LOW signal to D7
  //Hopefully digitalWrite will not muck up the GPS stuff. We may have to change this to a bitwise
  //operator. I don't quite remember how to do that but I can look it up if need be.
  //If this is one then autopilot is on for both
  //If this is set to 3 then only one aircraft will be on autopilot - FAILSAFE MODE REMOVED 1/16/18
  if ((autopilot == 1)) {
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    
    digitalWrite(HIGHLOWPIN,HIGH); //Using pin D7 to output high or low.

    //Pitch Angle - PD Controller
    float pitch = euler.y(); //pitch
    float pitch_rate = -rate.y(); //pitch rate
    float deltaPitch = kp_pitch*(pitchc-pitch)+kd_pitch*(pitch_ratec-pitch_rate);
    elevatorCommand = (deltaPitch + 100);
    
    //Waypoint Control

    //Crap what do we do about altitude. Throttle?
    if (sqrt(pow(xcommand-GPS.x,2) + pow(ycommand-GPS.y,2)) < BUBBLE) { 
      WPcounter++;
      if (WPcounter > 3) {
        WPcounter = 0;
      }
      xcommand = WPx[WPcounter];
      ycommand = WPy[WPcounter];
    }  
    
    //Yaw angle controller - PD Controller  
    //Debugging Yaw Angle Controller
    //psi_command = 0;
    //dpsi = (heading-psi_command);

    //Actual Yaw Angle Controller
    psi_command = atan2(ycommand-y,xcommand-x);
    float spsi = sin(heading);
    float cpsi = cos(heading);
    float spsic = sin(psi_command);
    float cpsic = cos(psi_command);
    dpsi = atan2(spsi*cpsic-cpsi*spsic,cpsi*cpsic+spsi*spsic); //this entire dpsi crap is to make sure that our dpsi is between -180 and 180
    
    //float rollc = (70.0/180.0)*dpsi + 0.0*(rate.x());    
    
    //For debugging roll controller
    float rollc = 0;

    //Saturation block
    if (rollc > 45) {
      rollc = 45;
    }
    if (rollc < -45) {
      rollc = 45;
    }

    //Roll Angle - PD Controller - euler.z is negative roll
    float roll = -euler.z(); //roll
    float roll_rate = rate.x(); //roll rate
    float deltaRoll = kp_roll*(rollc-roll)+kd_roll*(roll_ratec-roll_rate);
    aileronCommand = (deltaRoll + 100);

    //Saturation Block on Servos so we don't break anything
    if (aileronCommand >= 145) {
      aileronCommand = 145;
    }
    if (aileronCommand <= 35) {
      aileronCommand = 35;
    }
    if (elevatorCommand >= 145) {
      elevatorCommand = 145;
    }
    if (elevatorCommand <= 35) {
      elevatorCommand = 35;
    }

    //Perform the doublet manuever 
    //if (autopilot == 2) {
    //  float diff = millis()/1000.0-autoOn;
    //  if (diff < 0.5) {
    //    aileronCommand = 45;
    //  } 
    //  else if ((diff > 0.5) and (diff < 2*0.5)) {
    //    aileronCommand = 140;
    //  }
    //}
    
    aileron.write(aileronCommand);
    elevator.write(elevatorCommand);
  }
  else {
    digitalWrite(HIGHLOWPIN,LOW);
  }

  //printing if clause
  if (millis()/1000.0 > 0.1 + lastPrint) {

    if (!autopilot) {
      euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
      rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    }
    UpdateHeading(euler.x(),GPS.angle); //this is our new UpdateHeading control law
    
    lastPrint = millis()/1000.0; // reset timer

    // PITOT //
    int pitotValue = analogRead(PITOTPIN); //Pitot in analog 9 - I think Rockwell put the Pitot in A9 not A14 on the PCB. Will have to check
   
    //SERIAL PRINTS
    //GPS Sensor
    if (PRINTSERIAL) {
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
    Serial.print(GPS.speed); Serial.print(" "); //This is speed in knots
    Serial.print(GPS.angle); Serial.print(" "); //According to this website
    Serial.print(GPS.altitude); Serial.print(" "); //http://www.gpsinformation.org/dale/nmea.htm

    //IMU Data
    Serial.print(euler.x()); Serial.print(" "); //positive yaw
    Serial.print(euler.y()); Serial.print(" "); //positive pitch
    Serial.print(euler.z()); Serial.print(" "); //negative roll
    Serial.print(rate.x()); Serial.print(" ");
    Serial.print(rate.y()); Serial.print(" ");
    Serial.print(rate.z()); Serial.print(" ");
    
    //PITOT SENSOR
    Serial.print(pitotValue); Serial.print(" ");
   
    //Servos and autopilot health
    Serial.print(pulse_timeTrainer); Serial.print(" ");
    Serial.print(autopilot); Serial.print(" ");
    Serial.print(aileronCommand); Serial.print(" ");
    Serial.print(elevatorCommand); Serial.print(" ");
    Serial.print(psi_command); Serial.print(" ");
    //Serial.print(backup_receiver); Serial.print(" ");
    Serial.print(heading); Serial.print(" ");
    Serial.print(GPS.x); Serial.print(" ");
    Serial.print(GPS.y); Serial.print(" ");
    Serial.print(xcommand); Serial.print(" ");
    Serial.print(ycommand); Serial.print(" ");
    Serial.print("\n");
    }
  
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
    myFile.print(GPS.angle); myFile.print(" "); //-- so this is GPS heading
    myFile.print(GPS.altitude); myFile.print(" ");
  
    //IMU Data
    myFile.print(euler.x()); myFile.print(" ");
    myFile.print(euler.y()); myFile.print(" ");
    myFile.print(euler.z()); myFile.print(" "); //-- this is the IMU heading
    myFile.print(rate.x()); myFile.print(" ");
    myFile.print(rate.y()); myFile.print(" ");
    myFile.print(rate.z()); myFile.print(" ");
    
    //PITOT
    myFile.print(pitotValue); myFile.print(" ");
    
    //Servos and autopilot health
    myFile.print(pulse_timeTrainer); myFile.print(" ");
    myFile.print(autopilot); myFile.print(" ");
    myFile.print(aileronCommand); myFile.print(" ");
    myFile.print(elevatorCommand); myFile.print(" ");
    myFile.print(psi_command); myFile.print(" ");
    // --- this is the stitched together heading using the UpdatedHeading routine which takes GPS and IMU heading
    myFile.print(heading); myFile.print(" "); 
    myFile.print(GPS.x); myFile.print(" ");
    myFile.print(GPS.y); myFile.print(" ");
    myFile.print(xcommand); myFile.print(" ");
    myFile.print(ycommand); myFile.print(" ");
    myFile.print("\n");
   
    //Flush the file
    myFile.flush();
  }
}

//As of 9/18/2017 - this has not been flight tested
//What you should do to test this is go outside and test the gains while only outputting GPS.angle and psi_IMU in a two column
//format. That way you can debug the gains
void UpdateHeading(float psi_IMU,float psi_GPS) {
  //Wrapper Checks
  if (psi_IMU > 180) {
    psi_IMU -= 360;
  }
  if (psi_GPS > 180) {
    psi_GPS -= 360;
  }
  //Update heading with delta from IMU
  heading += (psi_IMU - psi_IMU_prev);
  psi_IMU_prev = psi_IMU;
  //Update heading if psi_GPS is new
  float KF = 0.2; //KF must be between 0 and 1
  if (psi_GPS != psi_GPS_prev) {
     heading += KF*(psi_GPS-heading);
     psi_GPS_prev = psi_GPS;
  }
  //Wrapper checks
  if (heading < -180) {
    heading += 360;
  }
  if (heading > 180) {
    heading -= 360;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
ISR(PCINT0_vect) { //for PORTB0-7

  current_time = micros();

  //We are now reading the trainer switch from pin 10
  if (PINB & B00010000) {
    if (last_channelTrainer == 0) {
      last_channelTrainer = 1;
      timerTrainer = current_time;
    }
  }
  else if (last_channelTrainer == 1) {
      last_channelTrainer = 0;
      pulse_timeTrainer = current_time - timerTrainer;
  }

}
