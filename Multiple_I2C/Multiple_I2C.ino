// FASTSensor sensor package includes GPS, 10DOF, Accelerometer, SD card, and reads servo signals.


// 9 DOF //
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);


//IMU//
// The 10DOF Library Includes the Libraries for Each of the Sensors
#include <Adafruit_10DOF.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

float temperature;
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

//Arduino Time
float lastPrint = 0;

//GPS//
#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial1);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


    // Servo Control Allows the FASTSensor to be Used as a Flight Controller

//Servo//
#include <Servo.h>
int b = 0;
int d = 0;
int del_theta = 0;
int Servo_Angle = 0;
int Servo_Angle2 = 0;

float start_time = 0;

Servo myservo;
Servo myservo2;
//Servo AILE,ELEV,THRO; //aileron, elevator, throttle servos.. just reading for now
//unsigned long current_time,timerA,timerE;
//byte last_channelA,last_channelE;
//int pulse_timeA,pulse_timeE;
  
//SD//
#include <SPI.h> // apparently I need this too
#include <SD.h>
File myFile;

// ADXL //
int xpin = 2;
int ypin = 1;
int zpin = 0;


// Initialization for Velo Calculation //

int c = 0;
float R = 8.31447;
float L = 0.0065;
float g = 9.81;
float M = 0.0289644;
float Altitude_Last = -999;
float Velocity_Last = 0;
float Velocity_New;
unsigned long time_last = 0;
float Velocity;
float Altitude;
float Ground_Pressure;
float Ground_Temperature;
// This came from the GitHub 10DOF Code
  // It Displays Details of Each Sensor

//void displaySensorDetails(void)
//{
//  sensor_t sensor;
//  
//  accel.getSensor(&sensor);
//  Serial.println(F("----------- ACCELEROMETER ----------"));
//  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
//  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
//  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
//  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" m/s^2"));
//  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" m/s^2"));
//  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" m/s^2"));
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
//
//  gyro.getSensor(&sensor);
//  Serial.println(F("------------- GYROSCOPE -----------"));
//  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
//  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
//  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
//  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" rad/s"));
//  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" rad/s"));
//  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" rad/s"));
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
//  
//  mag.getSensor(&sensor);
//  Serial.println(F("----------- MAGNETOMETER -----------"));
//  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
//  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
//  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
//  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" uT"));
//  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" uT"));
//  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" uT"));  
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
//
//  bmp.getSensor(&sensor);
//  Serial.println(F("-------- PRESSURE/ALTITUDE ---------"));
//  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
//  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
//  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
//  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" hPa"));
//  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" hPa"));
//  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" hPa"));  
//  Serial.println(F("------------------------------------"));
//  Serial.println(F(""));
  
//}


///////////////////////////////////////////////////////////////////////////////////////
void setup() 
{


  //  Servo //

  myservo.attach(8);
  myservo2.attach(9);
  myservo.write(130);
  myservo2.write(130);
  
  // SD //
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (53 on the Mega) must be left as an output or the SD library functions will not work. 
  pinMode(53, OUTPUT);
  
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while(1){};
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  char filename[15];
  strcpy(filename, "FAST_00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
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
  
  
  // Servo //
  // Writing to pins 8 9 and 10
  // I just need to read for now
  // and the writing will need autonoumus flight
  //AILE.attach(8);
  //ELEV.attach(9);
  //THRO.attach(10);
  
  
  // 10DOF//
  Serial.begin(115200);
  Serial.println(F("Adafruit 10DOF Tester")); Serial.println("");
  
  /* Initialise the sensors */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
//  displaySensorDetails();


  // 9 DOF  //

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
 
  // GPS //
  GPS.begin(9600);
  
  Serial.print("GPS Class Initialized \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //turn on RMC(recommended minimum)
  Serial.print("NMEA OUTPUT Set \n");
  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  // 1 or 10 Hz update rate. what should i use??
  
  Serial.print("Update Rate Set \n");
  
  useInterrupt(false);
  
  Serial.print("Delaying for  1 second \n");
  delay(1000);
  
  lastPrint = millis()/1000.0;
}


/////////////////////////////////////////////////////////////////////////////////////
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMERO_COMPA_vect) {
  char c = GPS.read();
  if (GPSECHO)
    if (c) UDR0 = c; // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
    }
}

uint32_t timer = millis();


///////////////////////////////////////////////////////////////////////////////////////
void loop() {



  
   // GPS //
  if (! usingInterrupt) {
     // read data from the GPS in the 'main loop'
     char c = GPS.read();
     // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
      //if (c) UDR0 = c; //writing direct to UDR0 is much faster than Serial.print
      //but only one character can be written at a time.
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
    return;
  }

  



// Printing if clause 
  // Data Taken Every Tenth of a Second
  
      if (millis()/1000.0 > 0.1 + lastPrint) {

        lastPrint = millis()/1000.0;
    
//  
    // Serial Prints //
    
      Serial.print("\nTime: ");
      Serial.print(GPS.hour, DEC); Serial.print(':');
      Serial.print(GPS.minute, DEC); Serial.print(':');
      Serial.print(GPS.seconds, DEC); Serial.print(':');
      Serial.print(GPS.milliseconds); Serial.print(" ");
      Serial.print("Fix: "); Serial.print((int)GPS.fix); Serial.print(" ");
      Serial.print(GPS.latitudeDegrees, 8); Serial.print(" ");
      Serial.print(GPS.longitudeDegrees, 8); Serial.print(" "); 
      Serial.print("Speed (knots): ");Serial.print(GPS.speed); Serial.print(" ");
      Serial.print("Angle: "); Serial.print(GPS.angle); Serial.print(" ");
      Serial.print("Altitude: "); Serial.print(GPS.altitude); Serial.print(" ");
     
    // File Prints //
      
      myFile.print(GPS.hour, DEC); myFile.print(' ');
      myFile.print(GPS.minute, DEC); myFile.print(' ');
      myFile.print(GPS.seconds, DEC); myFile.print(' ');
      myFile.print(lastPrint); myFile.print(" ");
      myFile.print((int)GPS.fix); myFile.print(" ");
      myFile.print(GPS.latitudeDegrees, 8); myFile.print(" ");
      myFile.print(GPS.longitudeDegrees, 8); myFile.print(" "); 
      myFile.print(GPS.speed); myFile.print(" ");
      myFile.print(GPS.angle); myFile.print(" ");
      myFile.print(GPS.altitude); myFile.print(" ");

//
//        //Accelerometer
//      Serial.print(x_accel); Serial.print(" ");
//      Serial.print(y_accel); Serial.print(" ");
//      Serial.print(z_accel); Serial.print("\n");

//  10DOF Data  //    
  /* Get a new sensor event */
      sensors_event_t event;
   

// It will break if serial print and file print are not called consecutively 

// IMU Accelerometer //
  /* Display the results (acceleration is measured in m/s^2) */
  
      accel.getEvent(&event);

    // Serial Prints //
    
      Serial.print(F("ACCEL "));
      Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
      Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
      Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.print("m/s^2 ");
    
    // File Prints //
    
      myFile.print(event.acceleration.x); myFile.print(" ");
      myFile.print(event.acceleration.y); myFile.print(" ");
      myFile.print(event.acceleration.z); myFile.print(" ");


//  Magnetometer  //
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  
      mag.getEvent(&event);

    // Serial Prints //
    
      Serial.print(F("MAG   "));
      Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
      Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
      Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.print("uT ");

    // File Prints //

      myFile.print(event.magnetic.x); myFile.print(" ");
      myFile.print(event.magnetic.y); myFile.print(" ");
      myFile.print(event.magnetic.z); myFile.print(" ");


//  Gyro  //
    /* Display the results (gyrocope values in rad/s) */
    
      gyro.getEvent(&event);

    // Serial Prints //
    
      Serial.print(F("GYRO  "));
      Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
      Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
      Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");Serial.print("rad/s ");  

    // File Prints //

      myFile.print(event.gyro.x); myFile.print(" ");
      myFile.print(event.gyro.y); myFile.print(" ");
      myFile.print(event.gyro.z); myFile.print(" ");

  
//  Pressure and Temp //

  /* Display atmospheric pressure in hPa */
  /* Display ambient temperature in C */  
  
      bmp.getEvent(&event);
        if (event.pressure)
        {
 
    // Serial Prints //
       
         Serial.print(F("PRESS "));
         Serial.print(event.pressure);
         Serial.print(F(" hPa, "));

         bmp.getTemperature(&temperature);
         Serial.print(temperature);
         Serial.print(F(" C, "));
         /* Then convert the atmospheric pressure, SLP and temp to altitude    */
         /* Update this next line with the current SLP for better results      */
         //Serial.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure,temperature)); 
        // Serial.print(F(" m"));

    // File Prints //

         myFile.print(temperature); myFile.print(" ");
         myFile.print(event.pressure); myFile.print(" ");
        // myFile.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure,temperature));
        
        if (millis() > 20000 && c == 0)
        {
            Ground_Pressure = event.pressure;
            Ground_Temperature = temperature + 273.13;
            c = 1;
        }

        if (c == 1)
        {
           Altitude = (1- pow(event.pressure/Ground_Pressure, R*L/(g*M)))*(Ground_Temperature/L)*3.28;
           if (Altitude_Last == -999) {
            Altitude_Last = Altitude;
            Velocity_New = 0;
           }
           else {
            Velocity_New = abs(Altitude - Altitude_Last)/(millis()/1000.0 - time_last);
            myFile.print(Altitude); myFile.print(" ");
            // myFile.print(Altitude_Last); Serial.print(" ");
            // myFile.print(millis()/1000.0-time_last);Serial.print(" ");
//            Serial.print(time_last);Serial.print(" ");
//            Serial.print(millis());Serial.print(" ");
            Altitude_Last = Altitude;
           }
           time_last = millis()/1000.0;
           Velocity = (Velocity_New+Velocity_Last)/2.0;
           myFile.print(Velocity); myFile.print(" ");
           Velocity_Last = Velocity;
        } 
        
           
           

           if (Velocity > 250 && b == 0)
           {
            b = 1;
           }

           if (Velocity < 200 && b == 1 && d == 0)
           //if (d==0){
           {
  
            start_time = millis();
            d == 1;
           }
           
           if (abs(millis()-start_time) < 3000 && d == 1)
            {
            del_theta = -45;
            Servo_Angle = 100+del_theta;
            Servo_Angle2 = 120+del_theta;
            myservo.write(Servo_Angle);
            myservo2.write(Servo_Angle2);
            myFile.print(Servo_Angle); myFile.print(" ");
            myFile.print(Servo_Angle2); myFile.print(" ");
            }
          else if (abs(millis()-start_time) >= 3000 && abs(millis()-start_time) < 5000 && d == 1)
            {
            del_theta = 45;
            Servo_Angle = 100+del_theta;
            Servo_Angle2 = 120+del_theta;
            myservo.write(Servo_Angle);
            myservo2.write(Servo_Angle2);
            myFile.print(Servo_Angle); myFile.print(" ");
            myFile.print(Servo_Angle2); myFile.print(" ");
            }
          else
            {
            del_theta = 0;
            Servo_Angle = 100+del_theta;
            Servo_Angle2 = 120+del_theta;
            myservo.write(Servo_Angle);
            myservo2.write(Servo_Angle2);
            myFile.print(Servo_Angle); myFile.print(" ");
            myFile.print(Servo_Angle2); myFile.print(" ");
            }
           
           
           
           
      }


   imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> rate = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  //imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  int8_t temp = bno.getTemp();


 // 9DOF File Prints  // 
   myFile.print(euler.x()); myFile.print(" ");
   myFile.print(euler.y()); myFile.print(" ");
   myFile.print(euler.z()); myFile.print(" ");
   myFile.print(rate.x()); myFile.print(" ");
   myFile.print(rate.y()); myFile.print(" ");
   myFile.print(rate.z()); myFile.print(" ");
   myFile.print(lin_accel.x()); myFile.print(" ");
   myFile.print(lin_accel.y()); myFile.print(" ");
   myFile.print(lin_accel.z()); myFile.print(" ");
   myFile.print(temp);
   
   
//  Servos   //
//    Serial.print(pulse_timeA); Serial.print(" ");
//    Serial.print(pulse_timeE); Serial.print("\n");
//  
//  Servos   //
//    myFile.print(pulse_timeA); myFile.print(" ");
//    myFile.print(pulse_timeE); myFile.print("\n");
//   


//  ADXL    //
//      myFile.print(x_accel); myFile.print(" ");
//      myFile.print(y_accel); myFile.print(" ");
//      myFile.print(z_accel); myFile.println(" ");
  
   

    //Flush the file
       myFile.println();
       myFile.flush();
      
      }
    
}


//////////////////////////////////////////////////////////////////////////////////////
//ISR(PCINT0_vect) { //for PORTB0-7
//
//  current_time = micros();
//
//  //Reading elevator from pin 12
//  if (PINB & B01000000) { 
//  
//    if(last_channelE == 0)
//    {
//      last_channelE = 1;
//      timerE = current_time;
//    }
//  }
//  else if(last_channelE == 1){
//      last_channelE = 0;
//      pulse_timeE = current_time - timerE;
//  }
//
// //Reading aileron from pin 11
// if (PINB & B00100000) { 
//  
//    if(last_channelA == 0)
//    {
//      last_channelA = 1;
//      timerA = current_time;
//    }
//  }
//  else if(last_channelA == 1){
//      last_channelA = 0;
//      pulse_timeA = current_time - timerA;
//  }
//
//}
