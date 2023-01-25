#include "gps.h"

Montalvo_GPS::Montalvo_GPS() {
}

void Montalvo_GPS::setup_GPS() {
  GPS = new Adafruit_GPS(&Serial1);
  //GPS
  #if PRINTSERIAL
  Serial.println("Initializing GPS...");
  #endif
  GPS->begin(9600);
  Serial1.begin(9600);
  GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  #if PRINTSERIAL
  Serial.println("GPS Intialized");
  #endif
}

void Montalvo_GPS::poll_GPS() {
  //So every loop we will read one character.
  char c = 1;
  while (c != 0) {
    c = GPS->read(); //so this c here doesn't do anything in the ino but it does do something
    //in the library Adafruit_GPS. So let's see if we can mess with it here.
    #ifdef GPSDEBUG
    if (c == 0) {
      Serial.println("brk");
    } else {
      Serial.println("cnt");
    }
    #endif
  }
  //However we will only parse when we have a full sentence
  //Hopefully this doesn't fuck with any timers or anything like it did
  //on the UNO.
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS->newNMEAreceived()) {
    GPS->parse(GPS->lastNMEA());
  }
}

void Montalvo_GPS::print_GPS() {
  Serial.print(GPS->latitudeDegrees,8);
  Serial.print(" ");
  Serial.print(GPS->longitudeDegrees,8);
  Serial.print(" ");
  Serial.print(GPS->hour);
  Serial.print(" ");
  Serial.print(GPS->minute);
  Serial.print(" ");
  Serial.print(GPS->seconds);
  Serial.print(" ");
  Serial.print(GPS->satellites);
}

void Montalvo_GPS::print_SD_GPS(File myFile) {
  myFile.print(GPS->latitudeDegrees,8); 
  myFile.print(" ");
  myFile.print(GPS->longitudeDegrees,8);
  myFile.print(" ");
  myFile.print(GPS->speed);
  //Can also get year,month,day,hour,minute,seconds,fix,angle,altitude
}

