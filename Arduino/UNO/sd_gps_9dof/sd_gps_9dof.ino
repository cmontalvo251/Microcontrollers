// Meta Aircraft sensor package includes GPS, 9DOF, pitot probe, 

//IMU//
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

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
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//Servo//
#include <Servo.h>
Servo AILE,ELEV,THRO; //aileron, elevator, throttle servos.. just reading for now
unsigned long current_time,timerA,timerE;
byte last_channelA,last_channelE;
int pulse_timeA,pulse_timeE;
  
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
  strcpy(filename, "META000.TXT");
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
  
  
  // Servo //
  // Writting to pins 8 9 and 10
  // I just need to read for now
  // and the writting will need autonoumus flight
  //AILE.attach(8);
  //ELEV.attach(9);
  //THRO.attach(10);
  
  // Reading from pins 11 12
  PCICR |= (1 << PCIE0); //PORTB 0-7 // not sure what this is
  PCMSK0 |= (1 << PCINT5); //PORTB5 - Digital Pin 11  // why ('|=')
  PCMSK0 |= (1 << PCINT6); //PORTB6 - Digital Pin 12
  
  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
    
  bno.setExtCrystalUse(true);
 
 
  // GPS //
  GPS.begin(9600);
  
  Serial.print("GPS Class Initialized \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //turn on RMC(recommended minimum)
  Serial.print("NMEA OUTPUT Set \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);  // 1 or 10 Hz update rate. what should i use??
  
  Serial.print("Update Rate Set \n");
  
  useInterrupt(false);
  
  Serial.print("Delaying for  1 second \n");
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
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
    return;
  }
  
  
  // PITOT //
  float pitotValue = analogRead(14); //Pitot in analog 14
  
  
  //printing if clause
  if (millis()/1000.0 > 0.1 + lastPrint) {
    
    lastPrint = millis()/1000.0; // reset timer
   
    // does this need to be in this if clause?
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    int8_t temp = bno.getTemp();
  
    //SERIAL PRINTS
    //GPS Sensor
    Serial.print(GPS.hour, DEC); Serial.print(' ');
    Serial.print(GPS.minute, DEC); Serial.print(' ');
    Serial.print(GPS.seconds, DEC); Serial.print(' ');
    Serial.print(lastPrint); Serial.print(" ");
    Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
    Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
    Serial.print(GPS.speed); Serial.print(" ");
    Serial.print(GPS.angle); Serial.print(" ");
    Serial.print(GPS.altitude); Serial.print(" ");

    //IMU Data
    Serial.print(euler.x()); Serial.print(" ");
    Serial.print(euler.y()); Serial.print(" ");
    Serial.print(euler.z()); Serial.print(" ");
    Serial.print(rate.x()); Serial.print(" ");
    Serial.print(rate.y()); Serial.print(" ");
    Serial.print(rate.z()); Serial.print(" ");
    Serial.print(lin_accel.x()); Serial.print(" ");
    Serial.print(lin_accel.y()); Serial.print(" ");
    Serial.print(lin_accel.z()); Serial.print(" ");
    Serial.print(temp); Serial.print(" ");
   
    //PITOT SENSOR
    Serial.print(pitotValue);
   
    //Servos
    Serial.print(pulse_timeA);
    Serial.print(pulse_timeE);
    Serial.print("\n");
  
  
   /* Write data to file */
   //GPS Sensor
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
   myFile.print(lin_accel.x()); myFile.print(" ");
   myFile.print(lin_accel.y()); myFile.print(" ");
   myFile.print(lin_accel.z()); myFile.print(" ");
   myFile.print(temp); myFile.print(" ");
   
   //PITOT
   myFile.print(pitotValue);
   
   //Servos
    myFile.print(pulse_timeA);
    myFile.print(pulse_timeE);
    myFile.print("\n");
   
    //Flush the file
    myFile.flush();
  }
}


//////////////////////////////////////////////////////////////////////////////////////
ISR(PCINT0_vect) { //for PORTB0-7

  current_time = micros();

  //Reading elevator from pin 12
  if (PINB & B01000000) { 
  
    if(last_channelE == 0)
    {
      last_channelE = 1;
      timerE = current_time;
    }
  }
  else if(last_channelE == 1){
      last_channelE = 0;
      pulse_timeE = current_time - timerE;
  }

 //Reading aileron from pin 11
 if (PINB & B00100000) { 
  
    if(last_channelA == 0)
    {
      last_channelA = 1;
      timerA = current_time;
    }
  }
  else if(last_channelA == 1){
      last_channelA = 0;
      pulse_timeA = current_time - timerA;
  }

}
