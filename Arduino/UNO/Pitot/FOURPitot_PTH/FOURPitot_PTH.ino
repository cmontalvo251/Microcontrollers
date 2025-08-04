#include <GPS_SIMPLE.h>
#include <SD.h>
#include <BME280_SIMPLE.h>

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

Adafruit_BME280 bme;

File logfile;
#define PRINTSERIAL 1

#define NUMPITOT 4
float lastPrint;

void setup() {
  
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  
  pinMode(10, OUTPUT);
  
  if (!SD.begin(10)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
  }
  char filename[15];
  strcpy(filename, "GPSLO000.TXT");
  for (int i = 0; i < 1000; i++) {
    filename[5] = '0' + i/100;
    filename[6] = '0' + (i/10)%10;
    //filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  //char lastfile[15];
  //strcpy(lastfile,"GPSLOG99.TXT"); -- Do not uncomment this. This will break the code
  //if (SD.exists(lastfile)){
  //  Serial.print("Sorry SD card has reached its naming limit. Suggest wiping SD card");
  //  while(1){};
  //}

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  // connect to the GPS at the desired rate
  GPS.begin();

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 100 millihertz (once every 10 seconds), 1Hz or 5Hz update rate
  GPS.sendCommand(PGCMD_NOANTENNA);

  Serial.println("Ready!");

  //Setup BME280
  bme.begin();
  
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

    //Print GPS Time
    if (PRINTSERIAL) {
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print(" ");
    Serial.print(lastPrint); Serial.print(" ");

    //Print Important GPS info
    Serial.print(GPS.latitudeDegrees,8); Serial.print(" ");
    Serial.print(GPS.longitudeDegrees,8); Serial.print(" "); 
    // latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
    // latitudeDegrees += int(latitude/100);
    // longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
    // longitudeDegrees += int(longitude/100);
    Serial.print(-99.0); Serial.print(" ");
    Serial.print(-99.0); Serial.print(" ");
    Serial.print(GPS.altitude); Serial.print(" ");
    }

    //Log GPS Time
    logfile.print(GPS.hour, DEC); logfile.print(':');
    logfile.print(GPS.minute, DEC); logfile.print(':');
    logfile.print(GPS.seconds, DEC); logfile.print(" ");
    logfile.print(lastPrint); logfile.print(" ");

    //Print Important GPS info
    logfile.print(GPS.latitudeDegrees,8); logfile.print(" ");
    logfile.print(GPS.longitudeDegrees,8); logfile.print(" "); 
    logfile.print(-99.0); logfile.print(" ");
    logfile.print(-99.0); logfile.print(" ");
    logfile.print(GPS.altitude); logfile.print(" ");

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

    //Pressure/Temp/Humidity
    float temp = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float humidity = bme.readHumidity();
    
    if (PRINTSERIAL){
      Serial.print(temp); Serial.print(" "); //Celsius
      Serial.print(pressure); Serial.print(" "); //hPa
//      //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
      Serial.print(humidity); Serial.print(" "); //Percentage
      Serial.print(int(GPS.fix)); Serial.print(" ");
      Serial.print(GPS.day); Serial.print(" ");
      Serial.print(GPS.month); Serial.print(" ");
      Serial.print(17); Serial.print(" ");
      Serial.print("\n");
    }
    logfile.print(temp); logfile.print(" "); //Celsius
    logfile.print(pressure); logfile.print(" "); //hPa
//    //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    logfile.print(humidity); logfile.print(" "); //Percentage
    logfile.print(int(GPS.fix)); logfile.print(" ");
    logfile.print(GPS.day); logfile.print(" ");
    logfile.print(GPS.month); logfile.print(" ");
    logfile.print(17); logfile.print(" ");
    logfile.print("\n");
    logfile.flush();
  }

}


/* End code */

