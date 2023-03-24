#include <SD.h>
 
File myFile;

#define CSPIN 10

void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega/Due) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(CSPIN, OUTPUT); //This is usually 10.
 
  if (!SD.begin(CSPIN)) { //This is usually 10.
    Serial.println("initialization failed!");
    while(1){Serial.print("init failed....\n");delay(1000);};
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char filename[15];
  strcpy(filename, "TEST000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[4] = '0' + i/100;
    filename[5] = '0' + (i/10)%10;
    filename[6] = '0' + i%10;
    Serial.print("i = ");
    Serial.println(i);
    Serial.println(filename);
    delay(10);
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  char lastfile[15];
  strcpy(lastfile,"TEST999.TXT");
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
}
 
void loop()
{
  delay(1000);
  Serial.print("X = ");
  Serial.print(random(1,10));
  Serial.print("\n");
  myFile.print("X = ");
  myFile.print(random(1,10));
  myFile.print("\n");
  myFile.flush();
}
