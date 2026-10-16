// SD Card Example

//SD Card Stuff
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
  
  Serial.println("Running....");
}

void loop()                     // run over and over again
{
  //Output Timer and Proximity Data
  Serial.print(millis()); Serial.print(" ");
  Serial.print("\n");

  //SD CARD LOG PRINTS
  myFile.print(millis()); myFile.print(" ");
  myFile.print("\n");
   
  //Flush the file
  myFile.flush();

  delay(1000);

}
