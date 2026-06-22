// Basic demo for accelerometer/gyro readings from Adafruit LSM6DS33
#include <Adafruit_LSM6DS33.h>
Adafruit_LSM6DS33 lsm6ds33;

//and writing to an SD card
#include <SD.h>
File myFile;
/*
ARD mSD
5v  5v
--  3v
gnd gnd
52 CLK
50 DO
51 DI
53 CS  
-- CD
*/

//and using a GPS
#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial1); 

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
//I recommend setting this to true
boolean usingInterrupt = true;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

void setup(void) {
  Serial.begin(115200);
  Serial.println("Adafruit LSM6DS33+GPS Datalogger!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  Serial.print("GPS Class Initialized \n");
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  Serial.print("NMEA OUTPUT Set \n");
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);   // 1 or 10 Hz update rate
  Serial.print("Update Rate Set \n");
  useInterrupt(usingInterrupt);
  
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
  
  //Now setup the accelerometer
  if (!lsm6ds33.begin_I2C()) {
    Serial.println("Failed to find LSM6DS33 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("LSM6DS33 Found!");

  //Set ranges and values
  // lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G); //2/4/8/16
  // lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS); //125/250/500/1000/2000/4000
  // lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  // lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2
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

void loop() {
  
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

  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);

  if (millis() - timer > 500) { //Be careful setting this update rate too low. Especially if you aren't using the interrupt for GPS.
    timer = millis();
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(" Latitude: ");
    Serial.print(GPS.latitudeDegrees,8);
    Serial.print(" Longitude: ");
    Serial.print(GPS.longitudeDegrees,8);
    //You can print more like speed, angle, altitude but I'll leave that to you 
    //if you want it.
  
    Serial.print(" Temperature ");
    Serial.print(temp.temperature);
    Serial.print(" deg C ");
  
    // Display the results (acceleration is measured in m/s^2) 
    Serial.print("Accel X: ");
    Serial.print(accel.acceleration.x);
    Serial.print(" Y: ");
    Serial.print(accel.acceleration.y);
    Serial.print(" Z: ");
    Serial.print(accel.acceleration.z);
    Serial.print(" m/s^2 ");
  
    // Display the results (rotation is measured in rad/s) 
    Serial.print(" Gyro X: ");
    Serial.print(gyro.gyro.x);
    Serial.print(" Y: ");
    Serial.print(gyro.gyro.y);
    Serial.print(" Z: ");
    Serial.print(gyro.gyro.z);
    Serial.println(" rad/s ");
    Serial.println();
  
    //SD CARD LOG PRINTS
    myFile.print(millis()); myFile.print(" ");
    myFile.print(accel.acceleration.x); myFile.print(" ");
    myFile.print(accel.acceleration.y); myFile.print(" ");
    myFile.print(accel.acceleration.z); myFile.print(" ");
    myFile.print(GPS.latitudeDegrees); myFile.print(" ");
    myFile.print(GPS.longitudeDegrees); myFile.print(" ");
    //Again you can print more that just acceleration and lat/lon
    //but I'll leave that to you to add it in. This at least has the bare minimum.
    myFile.print("\n");
     
    //Flush the file
    myFile.flush();
  }
}
