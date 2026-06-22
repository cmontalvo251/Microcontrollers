// Basic demo for accelerometer/gyro readings from Adafruit LSM6DS33
#include <Adafruit_LSM6DS33.h>
Adafruit_LSM6DS33 lsm6ds33;

//and writing to an SD card
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

// Basic demo for pressure readings from Adafruit LPS2X
#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
Adafruit_LPS22 lps;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Adafruit LSM6DS33 Datalogger!");

  // Try to initialize! LPS22
  if (!lps.begin_I2C()) {
    Serial.println("Failed to find LPS22 chip");
    while (1) { delay(10); }
  }
  Serial.println("LPS22 Found!");

  lps.setDataRate(LPS22_RATE_10_HZ); //Can also do 1,25,50,75
  Serial.print("Data rate set to 10 HZ ");

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
  
  //Now setup the accelerometer
  if (!lsm6ds33.begin_I2C()) {
    Serial.println("Failed to find LSM6DS33 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("LSM6DS33 Found!");

  //Set ranges and values
  // lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G); //2/4/8/16
  // lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS); //125/250/500/1000/2000/4000
  // lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  // lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2
}

void loop() {
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  //Read LPS22
  sensors_event_t temp22;
  sensors_event_t pressure;
  lps.getEvent(&pressure, &temp22);// get pressure and temp

  Serial.print("Temperature IMU/LPS22 ");
  Serial.print(temp.temperature);
  Serial.print("/");
  Serial.print(temp22.temperature);
  Serial.print(" deg C ");

  Serial.print(" Pressure: ");
  Serial.print(pressure.pressure);
  Serial.print(" hPa ");

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Accel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" Z: ");
  Serial.print(accel.acceleration.z);
  Serial.print(" m/s^2 ");

  /* Display the results (rotation is measured in rad/s) */
  Serial.print(" Gyro X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" Y: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" Z: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" rad/s ");
  Serial.println();

  //SD CARD LOG PRINTS
  myFile.print(millis()); myFile.print(" ");
  myFile.print(accel.acceleration.x); myFile.print(" ");
  myFile.print(accel.acceleration.y); myFile.print(" ");
  myFile.print(accel.acceleration.z); myFile.print(" ");
  myFile.print(pressure.pressure); myFile.print(" ");
  //You can obviously log more than this but that's left to the student
  //this at least is the bare minimum.
  myFile.print("\n");
   
  //Flush the file
  myFile.flush();

  delay(100);
}
