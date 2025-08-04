//#include <SPI.h>
//#include <Adafruit_GPS.h>
//#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>

// Ladyada's logger modified by Bill Greiman to use the SdFat library
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS Shield
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada
// Fllybob added 10 sec logging option
//SoftwareSerial mySerial(8, 7);
//Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
//#define GPSECHO  false
/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
//#define LOG_FIXONLY false  

// this keeps track of whether we're using the interrupt
// off by default!
//boolean usingInterrupt = false;
//void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// Set the pins used
#define chipSelect 10
#define ledPin 13

File logfile;
boolean printSerial = true;

//Everything needed for The Pitot Sensor
float sensorvalue;
int numPitot = 4; //Number of Pitot Sensors plugged in
int analogInPin[4]; //The # in brackets must be the same as the number of Pitot Probes
//Remember that pins on the arduino default to input therefore you don't need to run pinMode(#,INPUT)
float lastPrint = 0;
float offset = 0;
float Tottime1 = 0;

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A')+10;
}

// blink out an error code
void error(uint8_t errno) {
  /*
  if (SD.errorCode()) {
   putstring("SD error: ");
   Serial.print(card.errorCode(), HEX);
   Serial.print(',');
   Serial.println(card.errorData(), HEX);
   }
   */
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void setup() {
  // for Leonardos, if you want to debug SD issues, uncomment this line
  // to see serial output
  //while (!Serial);
  
  //For the 1 hole pitot sensor - Quad reference frame
  //A2 = North pitot probe 

  //For the 4 hole pitot sensor - Quad reference frame
  //A2 = West pitot probe
  //A3 = North Pitot Probe
  //A4 = South Pitot probe
  //A5 = East Pitot Probe

  //Thus columns on the SD card will be
  //West, North, South, East - Quad Reference frame

  //Note that typically I point the quad itself in the EAST Earth Coordinate direction
  //thus the SD card will actually have a different reference. However, to avoid confusion
  //I suggest you point the quad due north so that the pitot reference frame matches with the
  //Earth reference frame
  
  //Remember to verify this in the python code
  
  for (int idx = 0;idx<numPitot;idx++) {
    analogInPin[idx] = idx+2; //Airspeed sensor is hooked up to pins 2-5
  }

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  pinMode(ledPin, OUTPUT);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  //if (!SD.begin(chipSelect, 11, 12, 13)) {
  if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "GPSLO000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[5] = '0' + i/100;
    filename[6] = '0' + (i/10)%10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  /* char lastfile[15]; */
  /* strcpy(lastfile,"GPSLO999.TXT"); */
  /* if (SD.exists(lastfile)){ */
  /*   Serial.print("Sorry SD card has reached its naming limit. Suggest wiping SD card"); */
  /*   while(1){}; */
  /* } */

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  // connect to the GPS at the desired rate
  //GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For logging data, we don't suggest using anything but either RMC only or RMC+GGA
  // to keep the log files at a reasonable size
  // Set the update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 100 millihertz (once every 10 seconds), 1Hz or 5Hz update rate

  // Turn off updates on antenna status, if the firmware permits it
  //GPS.sendCommand(PGCMD_NOANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  //useInterrupt(false);

  Serial.println("Ready!");
  
  lastPrint = millis()/1000.0;
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
//SIGNAL(TIMER0_COMPA_vect) {
//  char c = GPS.read();
//  // if you want to debug, this is a good time to do it!
//  #ifdef UDR0
//      if (GPSECHO)
//        if (c) UDR0 = c;  
//      // writing direct to UDR0 is much much faster than Serial.print 
//      // but only one character can be written at a time. 
//  #endif
//}

//void useInterrupt(boolean v) {
//  if (v) {
//    // Timer0 is already used for millis() - we'll just interrupt somewhere
//    // in the middle and call the "Compare A" function above
//    OCR0A = 0xAF;
//    TIMSK0 |= _BV(OCIE0A);
//    usingInterrupt = true;
//  } 
//  else {
//    // do not call the interrupt function COMPA anymore
//    TIMSK0 &= ~_BV(OCIE0A);
//    usingInterrupt = false;
//  }
//}

void loop() {

  //Get GPS data
//  if (! usingInterrupt) {
//    // read data from the GPS in the 'main loop'
//    char c = GPS.read();
//    // if you want to debug, this is a good time to do it!
//    if (GPSECHO)
//      if (c) Serial.print(c);
//  }
  
  // if a sentence is received, we can check the checksum, parse it...
//  if (GPS.newNMEAreceived()) {
//    // a tricky thing here is if we print the NMEA sentence, or data
//    // we end up not listening and catching other sentences! 
//    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
//    
//    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
//      return;  // we can fail to parse a sentence in which case we should just wait for another
//      
//  }

    //Ok let's get the pitot sensor value
  //if (millis()/1000.0 > 0.01 + lastPrint) { //100 means it logs data at 10 Hz since it waits 100 ms or 0.1 seconds
    
    //Reset lastPrint
    lastPrint = millis()/1000.0;

    //Print GPS Time
//    if (printSerial) {
//    Serial.print(GPS.hour, DEC); Serial.print(':');
//    Serial.print(GPS.minute, DEC); Serial.print(':');
//    Serial.print(GPS.seconds, DEC); Serial.print(" ");
    Serial.print(lastPrint); Serial.print(" ");
//
//    //Print Important GPS info
//    Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
//    Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
//    Serial.print(GPS.speed); Serial.print(" ");
//    Serial.print(GPS.angle); Serial.print(" ");
//    Serial.print(GPS.altitude); Serial.print(" ");
//    }

    //Log GPS Time
//    logfile.print(GPS.hour, DEC); logfile.print(':');
//    logfile.print(GPS.minute, DEC); logfile.print(':');
//    logfile.print(GPS.seconds, DEC); logfile.print(" ");
     logfile.print(lastPrint); logfile.print(" ");
//
//    //Print Important GPS info
//    logfile.print(GPS.latitudeDegrees, 8); logfile.print(" ");
//    logfile.print(GPS.longitudeDegrees, 8); logfile.print(" "); 
//    logfile.print(GPS.speed); logfile.print(" ");
//    logfile.print(GPS.angle); logfile.print(" ");
//    logfile.print(GPS.altitude); logfile.print(" ");

    //Get Pitot sensor readings and print to file
    for (int idx = 0;idx<numPitot;idx++) {
      float sensorvalue = analogRead(analogInPin[idx]);
      if (printSerial) {
      Serial.print(sensorvalue);
      Serial.print(" ");
      }
      //Log to SD card
      logfile.print(sensorvalue);
      logfile.print(" ");
    }
//    if (printSerial){
//      Serial.print(GPS.day); Serial.print(" ");
//      Serial.print(GPS.month); Serial.print(" ");
//      Serial.print(GPS.year); Serial.print(" ");
      Serial.print("\n");
//    }
//    logfile.print(GPS.day); logfile.print(" ");
//    logfile.print(GPS.month); logfile.print(" ");
//    logfile.print(GPS.year); logfile.print(" ");
    logfile.print("\n");
    logfile.flush();
  //}

}


/* End code */

