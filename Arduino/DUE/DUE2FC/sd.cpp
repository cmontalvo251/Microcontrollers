#include "sd.h"
#include "gps.h"
#include "i2c.h"
#include "commands.h"
#include "userparams.h"

sdcard::sdcard() {
}

void sdcard::setup_SD() {
  //Initialize the SD Card
  #if PRINTSERIAL
  Serial.println("Initializing SD card...");
  #endif

  pinMode(44, OUTPUT);
  if (!SD.begin(44)) { 
    #if PRINTSERIAL
    Serial.println("initialization failed!");
    #endif
    while(1);
  }
  #if PRINTSERIAL
  Serial.println("initialization done.");
  #endif

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char filename[15];
  strcpy(filename, "DUE_000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[4] = '0' + i/100;
    filename[5] = '0' + (i/10)%10;
    filename[6] = '0' + i%10;
    #if PRINTSERIAL
    //Serial.print("i = ");
    //Serial.println(i);
    Serial.println(filename);
    #endif
    delay(10);
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (!myFile) {
    #if PRINTSERIAL
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    #endif
    while(1);
  }
  #if PRINTSERIAL
  Serial.print("Writing to "); 
  Serial.println(filename);
  #endif

}
