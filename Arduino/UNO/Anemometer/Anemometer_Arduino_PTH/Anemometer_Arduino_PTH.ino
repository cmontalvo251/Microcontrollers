#include <SPI.h>
#include <GPS_SIMPLE.h>
//#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>
#include <BME280_SIMPLE.h> //Uncommenting this leaves 461 bytes and code still works

Adafruit_BME280 bme; //Uncommenting this leaves 421 bytes and code still works

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

File logfile;
#define printSerial 1
float lastPrint = 0;

void setup() {
  
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
    while(1){};
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

  //char lastfile[15];
  //strcpy(lastfile,"GPSLO999.TXT");
  //if (SD.exists(lastfile)){
  //  Serial.print("Sorry SD card has reached its naming limit. Suggest wiping SD card");
  //  while(1){};
  //}

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while(1){};
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  // connect to the GPS at the desired rate
  GPS.begin(); //GPS_SIMPLE defaults to 9600 baud rate

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 100 millihertz (once every 10 seconds), 1Hz or 5Hz update rate
  GPS.sendCommand(PGCMD_NOANTENNA);

  //Setup BME280
  bme.begin(); //Uncommenting this leaves 352 bytes and the code still works. - 
  //Ok so it doesn't quite work but I got rid of some other stuff and got 360 bytes left over.

  Serial.println("Ready!");
  
  lastPrint = millis()/1000.0;
}

void loop() {

  //Read GPS data
  char c = GPS.read();
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
      
  }

    //Ok let's get the anemometer sensor value
  if (millis()/1000.0 > 0.2 + lastPrint) { //100 means it logs data at 10 Hz since it waits 100 ms or 0.1 seconds
    
    //Reset lastPrint
    lastPrint = millis()/1000.0;

    //Print GPS Time
    if (printSerial) {
    //Serial.print(GPS.lastNMEA()); Serial.print("\n");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print(" ");
    Serial.print(lastPrint); Serial.print(" ");

    //Print Important GPS info
    Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
    Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
    Serial.print(-99.0); Serial.print(" "); //speed
    Serial.print(-99.0); Serial.print(" "); //angle
    Serial.print(GPS.altitude); Serial.print(" ");
    }

    //Log GPS Time
    logfile.print(GPS.hour, DEC); logfile.print(':');
    logfile.print(GPS.minute, DEC); logfile.print(':');
    logfile.print(GPS.seconds, DEC); logfile.print(" ");
    logfile.print(lastPrint); logfile.print(" ");

    //Print Important GPS info
    logfile.print(GPS.latitudeDegrees, 8); logfile.print(" ");
    logfile.print(GPS.longitudeDegrees, 8); logfile.print(" "); 
    logfile.print(-99.0); logfile.print(" "); //speed
    logfile.print(-99.0); logfile.print(" "); //angle
    logfile.print(GPS.altitude); logfile.print(" ");

    //Get anemometer sensor readings and print to file
    float sensorVoltage = 0.004882814*analogRead(1); //Get a value between 0 and 1023 from the analog pin connected to the anemometer
    //float sensorVoltage = sensorvalue * voltageConversionConstant; //Convert sensor value to actual voltage
    //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
    float windSpeed;
    if (sensorVoltage <= 0.4){ 
      windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero. 
    } else { 
      windSpeed = ((sensorVoltage - 0.4)*50.0/(2.0 - 0.4)); //This is 32 online but Lisa S found on the spec sheet that it
      //is actually 50 m/s. The spec sheet can be found online. Not sure where but I trust her. Anyway 32*1.65 ~= 50 m/s 
      //which accounts for the scale factor error
    }   
    //Convert mph to m/s
    //windSpeed*=0.4407; - this was moved to two lines up
    //In order to make sure we don't output like a shit ton of data let's add a counter
    if (printSerial) {
      Serial.print(sensorVoltage);
      Serial.print(" ");
      Serial.print(windSpeed);
      Serial.print(" ");
    }
    //Log to SD card
    logfile.print(sensorVoltage);
    logfile.print(" ");
    logfile.print(windSpeed);
    logfile.print(" ");

    //Pressure/Temp/Humidity
    float temp = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float humidity = bme.readHumidity();
    
    if (printSerial){
      Serial.print(temp); Serial.print(" ");
      Serial.print(pressure); Serial.print(" ");
      Serial.print(humidity); Serial.print(" ");
      Serial.print(GPS.day); Serial.print(" ");
      Serial.print(GPS.month); Serial.print(" ");
      //Serial.print(17); Serial.print(" "); //year Make sure to fix this in 2018
      Serial.print("\n");
    }
    logfile.print(temp); logfile.print(" ");
    logfile.print(pressure); logfile.print(" ");
    logfile.print(humidity); logfile.print(" ");
    logfile.print(GPS.day); logfile.print(" ");
    logfile.print(GPS.month); logfile.print(" ");
    logfile.print(17); logfile.print(" "); //year Make sure to fix this in 2018
    logfile.print("\n");
    logfile.flush();
  }

}


/* End code */

