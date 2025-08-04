#include <Adafruit_GPS.h>
#include <SD.h>
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

imu::Vector<3> euler;
imu::Vector<3> rate;

const int chipSelect = 4;
Sd2Card card;

#define GPSECHO false

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);

File logfile;
#define PRINTSERIAL 1

#define NUMPITOT 8
float lastPrint;

void setup() {
  
  Serial.begin(115200);
  Serial.println("\r\n Lots of stuff here");

  //IMU SETUP
  if(!bno.begin())
  {
   /* There was a problem detecting the BNO055 ... check your connections */
   Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
   while(1);
  }
  delay(1000);
  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);

  //SD CARD SETUP
  //pinMode(10, OUTPUT);

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  
  if (!SD.begin(chipSelect)) {  
    Serial.println("Card init. failed!");
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

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  // connect to the GPS at the desired rate
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 100 millihertz (once every 10 seconds), 1Hz or 5Hz update rate
  GPS.sendCommand(PGCMD_NOANTENNA);

  Serial.println("Ready!");

  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);
  
  lastPrint = millis()/1000.0;
}

void loop() {

  //Get GPS data
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
      
  }

    //Ok let's get the pitot sensor value
  if (millis()/1000.0 > 0.2 + lastPrint) { //100 means it logs data at 10 Hz since it waits 100 ms or 0.1 seconds
    
    //Reset lastPrint
    lastPrint = millis()/1000.0;

    //IMU 
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

    //Print GPS Time
    if (PRINTSERIAL) {
    Serial.print(GPS.hour, DEC); Serial.print(' ');
    Serial.print(GPS.minute, DEC); Serial.print(' ');
    Serial.print(GPS.seconds, DEC); Serial.print(" ");
    Serial.print(lastPrint); Serial.print(" ");

    //Print Important GPS info
    Serial.print(GPS.latitudeDegrees,8); Serial.print(" ");
    Serial.print(GPS.longitudeDegrees,8); Serial.print(" "); 
    Serial.print(GPS.speed); Serial.print(" ");
    Serial.print(GPS.angle); Serial.print(" ");
    Serial.print(GPS.altitude); Serial.print(" ");

    Serial.print(euler.x()); Serial.print(" "); //positive yaw
    Serial.print(euler.y()); Serial.print(" "); //positive pitch
    Serial.print(euler.z()); Serial.print(" "); //negative roll
    Serial.print(rate.x()); Serial.print(" ");
    Serial.print(rate.y()); Serial.print(" ");
    Serial.print(rate.z()); Serial.print(" ");
    }

    //Log GPS Time
    logfile.print(GPS.hour, DEC); logfile.print(' ');
    logfile.print(GPS.minute, DEC); logfile.print(' ');
    logfile.print(GPS.seconds, DEC); logfile.print(" ");
    logfile.print(lastPrint); logfile.print(" ");

    //Print Important GPS info
    logfile.print(GPS.latitudeDegrees,8); logfile.print(" ");
    logfile.print(GPS.longitudeDegrees,8); logfile.print(" "); 
    logfile.print(GPS.speed); logfile.print(" ");
    logfile.print(GPS.angle); logfile.print(" ");
    logfile.print(GPS.altitude); logfile.print(" ");

    //IMU Data
    logfile.print(euler.x()); logfile.print(" ");
    logfile.print(euler.y()); logfile.print(" ");
    logfile.print(euler.z()); logfile.print(" "); //-- this is the IMU heading
    logfile.print(rate.x()); logfile.print(" ");
    logfile.print(rate.y()); logfile.print(" ");
    logfile.print(rate.z()); logfile.print(" ");

    //Get Pitot sensor readings and print to file
    for (int idx = 0;idx<NUMPITOT;idx++) {
      float sensorvalue = analogRead(idx);
      if (PRINTSERIAL) {
      Serial.print(sensorvalue);
      Serial.print(" ");
      }
      //Log to SD card
      logfile.print(sensorvalue);
      logfile.print(" ");
    }
    
    if (PRINTSERIAL){
      Serial.print(int(GPS.fix)); Serial.print(" ");
      Serial.print(GPS.day); Serial.print(" ");
      Serial.print(GPS.month); Serial.print(" ");
      Serial.print(GPS.year); Serial.print(" ");
      Serial.print("\n");
    }
    logfile.print(int(GPS.fix)); logfile.print(" ");
    logfile.print(GPS.day); logfile.print(" ");
    logfile.print(GPS.month); logfile.print(" ");
    logfile.print(GPS.year); logfile.print(" ");
    logfile.print("\n");
    logfile.flush();
  }

}


/* End code */

