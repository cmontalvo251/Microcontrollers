#include <SPI.h>
#include <SD.h>

//Make sure to select Adafruit Feather M0 - If you don't have that option you downloaded the wrong board.

File logfile;
#define printSerial 1
float lastPrint = 0;

void setup() {
  
  Serial.begin(115200);
  pinMode(10, OUTPUT); //What is this for?

  if (!SD.begin(4)) {      // 
    Serial.println("Card init. failed!");
    while(1){};
  }
  char filename[15];
  strcpy(filename, "RTSMZ000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[5] = '0' + i/100;
    filename[6] = '0' + (i/10)%10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while(1){};
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  Serial.println("Ready!");
  
  lastPrint = millis()/1000.0;
}

void loop() {

  //Ok let's get the anemometer sensor value
  if (millis()/1000.0 > 0.01 + lastPrint) { //100 means it logs data at 10 Hz since it waits 100 ms or 0.1 seconds
    
    //Reset lastPrint
    lastPrint = millis()/1000.0;

    Serial.print(millis()/1000.0);
    Serial.print("\n");

    logfile.print(millis()/1000.0);
    logfile.print("\n");
    logfile.flush();
  }

}


/* End code */

