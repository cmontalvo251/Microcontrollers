#include <SPI.h>
#include <SD.h>
#include <avr/sleep.h>

// Set the pins used
#define chipSelect 10
#define ledPin 13

File logfile;

//Everything needed for The Pitot Sensor
float sensorvalue;
int numPitot = 1; //Number of Pitot Sensors plugged in
int analogInPin[1]; //Airspeed sensor is hooked up to pins 1-4
//Remember that pins on the arduino default to input therefore you don't need to specify 
//pin 3 in setupPin
float lastTime = 0;

void setup() {
  // for Leonardos, if you want to debug SD issues, uncomment this line
  // to see serial output
  //while (!Serial);
  for (int idx = 0;idx<numPitot;idx++) {
    analogInPin[idx] = idx+1;
  }

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  //if (!SD.begin(chipSelect, 11, 12, 13)) {
  if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
    while(1);
  }
  char filename[15];
  strcpy(filename, "GPS__000.TXT");
  for (uint8_t i = 0; i < 1000; i++) {
    filename[5] = '0' + i/100;
    filename[6] = '0' + (i/10)%10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  /* char lastfile[15]; */
  /* strcpy(lastfile,"GPS__999.TXT"); */
  /* if (SD.exists(lastfile)){ */
  /*   Serial.print("Sorry SD card has reached its naming limit. Suggest wiping SD card"); */
  /*   while(1){}; */
  /* } */

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while(1);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  Serial.println("Ready!");
  
  lastTime = millis();

  delay(1000);
}

void loop() {
  //Ok let's get the pitot sensor value
  if (millis() > 100 + lastTime) { //100 means it logs data at 10 Hz since it waits 100 ms or 0.1 seconds
    logfile.print(millis()/1000.0);
    logfile.print(" ");
    Serial.print(millis()/1000.0);
    Serial.print(" ");
    for (int idx = 0;idx<numPitot;idx++) {
        float sensorvalue = analogRead(analogInPin[idx]);
        //In order to make sure we don't output like a shit ton of data let's add a counter
        logfile.print(sensorvalue);
        logfile.print(" ");
        Serial.print(sensorvalue);
        Serial.print(" ");
    }
    logfile.print("\n");
    Serial.print("\n");
    lastTime = millis();
    logfile.flush();
  }
}


/* End code */

