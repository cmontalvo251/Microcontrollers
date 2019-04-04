 // NALU SENSOR CODE
// Includes 3 Proximity Sensors
// SD Card 
// 10-Dof IMU with Temperature Sensor
// MTK GPS module using the Adafruit GPS module

// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

#include <Adafruit_GPS.h>

// If you're using a GPS module:
// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using software serial (sketch example default):
//   Connect the GPS TX (transmit) pin to Digital 3
//   Connect the GPS RX (receive) pin to Digital 2
// If using hardware serial (e.g. Arduino Mega):
//   Connect the GPS TX (transmit) pin to Arduino RX1, RX2 or RX3
//   Connect the GPS RX (receive) pin to matching TX1, TX2 or TX3

// If you're using the Adafruit GPS shield, change 
// SoftwareSerial mySerial(3, 2); -> SoftwareSerial mySerial(8, 7);
// and make sure the switch is set to SoftSerial

// If using software serial, keep this line enabled
// (you can change the pin numbers to match your wiring):
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(3, 2);
//   Connect the GPS TX (transmit) pin to Digital 3
//   Connect the GPS RX (receive) pin to Digital 2

// If using hardware serial (e.g. Arduino Mega), comment out the
// above SoftwareSerial line, and enable this line instead
// (you can change the Serial number to match your wiring):

//HardwareSerial mySerial = Serial1; //Turns out this doesn't work and
//Adafruit_GPS GPS(&mySerial); //Adafruit must have never tested it.
Adafruit_GPS GPS(&Serial1); //THE NALU SENSOR IS PLUGGED INTO SERIAL1. It used to be 3 but I resoldered it.

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//IMU Sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//Proximity Sensor
//#define frontTrigPin 47
//#define frontEchoPin 45

//#define leftTrigPin 46
//#define leftEchoPin 44

//#define rightTrigPin 49
//#define rightEchoPin 48

float lastPrint = 0;

//long cmright,cmleft;//,cmfront;

//SD Card Stuff
#include <SD.h>
 
File myFile;

int PRODUCTIONVERSION = 1;

void setup()  
{
    
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("NALU Sensor");

  //Initialize the SD Card
  pinMode(53, OUTPUT);
 
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while(1){};
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char filename[15];
  strcpy(filename, "NALU000.TXT");
  for (uint8_t i = 0; i < 1000; i++) {
    filename[4] = '0' + i/100;
    filename[5] = '0' + (i/10)%10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  char lastfile[15];
  strcpy(lastfile,"NALU999.TXT");
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

  /* Initialise the IMU sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");

  delay(1000);
    
  bno.setExtCrystalUse(true);

  //Setup the proximity Sensors
  //SetupPin(leftTrigPin,leftEchoPin);
  //SetupPin(frontTrigPin,frontEchoPin);
  //SetupPin(rightTrigPin,rightEchoPin);

  Serial.println("Proximity Sensors Setup");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  Serial.print("GPS Class Initialized \n");
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  Serial.print("NMEA OUTPUT Set \n");
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 or 10 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  Serial.print("Update Rate Set \n");

  //Request updates on antenna status
  //GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(false);

  Serial.print("Delaying for 1 second to ensure all sensors are onboard. \n");
  delay(1000);
  // Ask for firmware version
  //mySerial.println(PMTK_Q_RELEASE);

  Serial.println("Running....");

  lastPrint = millis()/1000.0;
}

//Setup routine for the Proximity Sensors
void SetupPin(int TrigPin,int EchoPin) {
 pinMode(TrigPin,OUTPUT);
 pinMode(EchoPin,INPUT); 
}

long Distance(int TrigPin,int EchoPin) {
  long duration; 
  digitalWrite(TrigPin,LOW);
  delayMicroseconds(2); 
  digitalWrite(TrigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin,LOW);
  duration = pulseIn(EchoPin,HIGH);
  long cm = (duration/2.0)/29.1;  
  if (cm > 100) {
    cm = 100;
  }
  return cm;
}  

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
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

uint32_t timer = millis();

void loop()                     // run over and over again
{
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
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

    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  //Output Timer and Proximity Data
  if (millis()/1000.0 > 0.05 + lastPrint) {

    lastPrint = millis()/1000.0;

   //Get Proximity Sensor Information

   ///Turn off the sonar sensors for speed to avoid aliasing during pumping
   //cmright = -99.0;
   //cmleft = -99.0;
   //cmfront = -99.0;
   
   //cmright = Distance(rightTrigPin,rightEchoPin);
   //cmleft = Distance(leftTrigPin,leftEchoPin);
   //cmfront = Distance(frontTrigPin,frontEchoPin);  

   //Get IMU Sensor information
   imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
   imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
   //imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
   //imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
   //int8_t temp = bno.getTemp();
   //int8_t temp = -99;
   
   //SERIAL PRINTS

   //GPS Sensor
   if (!PRODUCTIONVERSION) {
     Serial.print(GPS.hour, DEC); Serial.print(" ");
     Serial.print(GPS.minute, DEC); Serial.print(" ");
     Serial.print(GPS.seconds, DEC); Serial.print(" ");
     Serial.print(lastPrint); Serial.print(" ");
     Serial.print((int)GPS.fix); Serial.print(" ");
     Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
     Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
     Serial.print(GPS.speed); Serial.print(" ");
     Serial.print(GPS.angle); Serial.print(" ");
     Serial.print(GPS.altitude); Serial.print(" ");
     
     //Proximity Data
     //Serial.print(cmleft); Serial.print(" ");
     //Serial.print(cmfront); Serial.print(" ");
     //Serial.print(cmright); Serial.print(" ");

     //IMU Data
     Serial.print(euler.x()); Serial.print(" ");
     Serial.print(euler.y()); Serial.print(" ");
     Serial.print(euler.z()); Serial.print(" ");
     Serial.print(rate.x()); Serial.print(" ");
     Serial.print(rate.y()); Serial.print(" ");
     Serial.print(rate.z()); Serial.print(" ");
     //Serial.print(lin_accel.x()); Serial.print(" ");
     //Serial.print(lin_accel.y()); Serial.print(" ");
     //Serial.print(lin_accel.z()); Serial.print(" ");
     //Serial.print(temp); Serial.print(" ");
     //Serial.print(GPS.year); Serial.print(" ");
     Serial.print(GPS.month); Serial.print(" ");
     Serial.print(GPS.day); Serial.print(" ");
     //float Ox = 30.691914;
     //float Oy = -88.174853;
     //float GPSVAL = 6076.115485560000*0.3048*60.0;
     //float x = (GPS.latitudeDegrees-Ox)*GPSVAL;
     //float y = (GPS.longitudeDegrees-Oy)*GPSVAL*cos(Ox*PI/180.0);
     //Serial.print(x); Serial.print(" ");
     //Serial.print(y); Serial.print(" ");
     Serial.print("\n");
   }

   //SD CARD LOG PRINTS

   //GPS Sensor
   myFile.print(GPS.hour, DEC); myFile.print(" ");
   myFile.print(GPS.minute, DEC); myFile.print(" ");
   myFile.print(GPS.seconds, DEC); myFile.print(" ");
   myFile.print(lastPrint); myFile.print(" ");
   myFile.print((int)GPS.fix); myFile.print(" ");
   myFile.print(GPS.latitudeDegrees, 8); myFile.print(" ");
   myFile.print(GPS.longitudeDegrees, 8); myFile.print(" "); 
   myFile.print(GPS.speed); myFile.print(" ");
   myFile.print(GPS.angle); myFile.print(" ");
   myFile.print(GPS.altitude); myFile.print(" ");

   //Proximity Data
   //myFile.print(cmleft); myFile.print(" ");
   //myFile.print(cmfront); myFile.print(" ");
   //myFile.print(cmright); myFile.print(" ");

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
   //myFile.print(temp); myFile.print(" ");
   //myFile.print(GPS.year); myFile.print(" ");
   myFile.print(GPS.month); myFile.print(" ");
   myFile.print(GPS.day); myFile.print(" ");
   myFile.print("\n");
   
   //Flush the file
   myFile.flush();

  }
}
