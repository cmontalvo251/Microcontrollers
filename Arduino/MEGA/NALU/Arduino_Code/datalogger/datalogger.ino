 //Use NALU as a datalogger for any drone
// SD Card 
// 10-Dof IMU with Temperature Sensor
// MTK GPS module using the Adafruit GPS module

#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial1); //THE NALU SENSOR IS PLUGGED INTO SERIAL1.
#define GPSECHO  false
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//IMU Sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);
float lastPrint = 0;

//SD Card Stuff
#include <SD.h>
 
File myFile;

int PRODUCTIONVERSION = 0; //Set to true if you don't want it to print serial.

void setup()  
{
    
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  Serial.begin(115200);
  Serial.println("Data Logger");

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
  strcpy(filename, "DATA000.TXT");
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
  strcpy(lastfile,"DATA999.TXT");
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

  useInterrupt(false);

  Serial.print("Delaying for 1 second to ensure all sensors are onboard. \n");
  delay(1000);
  
  Serial.println("Running....");

  lastPrint = millis()/1000.0;
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
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  //Output Timer and Proximity Data
  if (millis()/1000.0 > 0.05 + lastPrint) {

    lastPrint = millis()/1000.0;

   //Get IMU Sensor information
   imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
   imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);   
   
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
     
     //IMU Data
     Serial.print(euler.x()); Serial.print(" ");
     Serial.print(euler.y()); Serial.print(" ");
     Serial.print(euler.z()); Serial.print(" ");
     Serial.print(rate.x()); Serial.print(" ");
     Serial.print(rate.y()); Serial.print(" ");
     Serial.print(rate.z()); Serial.print(" ");
     Serial.print(GPS.month); Serial.print(" ");
     Serial.print(GPS.day); Serial.print(" ");
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
   //IMU Data
   myFile.print(euler.x()); myFile.print(" ");
   myFile.print(euler.y()); myFile.print(" ");
   myFile.print(euler.z()); myFile.print(" ");
   myFile.print(rate.x()); myFile.print(" ");
   myFile.print(rate.y()); myFile.print(" ");
   myFile.print(rate.z()); myFile.print(" ");
   myFile.print(GPS.month); myFile.print(" ");
   myFile.print(GPS.day); myFile.print(" ");
   myFile.print("\n");
   
   //Flush the file
   myFile.flush();

  }
}
