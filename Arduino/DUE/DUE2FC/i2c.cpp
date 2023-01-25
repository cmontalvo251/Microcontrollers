#include "i2c.h"
#include "sd.h"

i2c::i2c() {
}

void i2c::setup_I2C() {
  bno = Adafruit_BNO055(55);

  //Start the Pressure Sensor
  mpl115a2.begin();

  #if PRINTSERIAL
  Serial.println("Pressure Sensor Initialized or so we think....");
  #endif

  // IMU //
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    #if PRINTSERIAL
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    #endif
    while(1);
  }

  #if PRINTSERIAL
  Serial.println("IMU Sensor Initialized - Waiting 1 second to calibrate - DO NOT MOVE!");
  #endif

  //Take like 100 pressure readings and average them
  int N = 100;
  for (int i = 0;i<N;i++) {
   sea_pressure += mpl115a2.getPressure();
  }
  sea_pressure/=float(N);
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}

void i2c::read_I2C(float loopTimer) {
  float dT = loopTimer - lastI2CTime; //Used for lowpass filter
  #ifdef DEBUGPRINTS
  Serial.print("Polling Sensors. Time = ");
  Serial.print(loopTimer);
  Serial.print(" Elapsed Time = ");
  Serial.println(dT,6);
  #endif
  lastI2CTime = millis()/1000.0;
  rate_fc = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  //lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  //lin_accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER); -- No longer needed
  //Need to verify this is right simply through experimentation
  //Verified - 12/6/2018
  //Az = -lin_accel.z(); -- No longer needed
  roll_rate = -rate_fc.y();
  pitch_rate = -rate_fc.x();
  yaw_rate = -rate_fc.z();
  roll = PI/180.0*(euler.y()); 
  pitch = PI/180.0*(euler.z()-9); //The 9.0 is because the sensor is not mounted flat on the PCB
  yaw = PI/180.0*euler.x();
  //Grab pressure data
  pressure = mpl115a2.getPressure();
  temperature = mpl115a2.getTemperature();
  //Take a look at KyrelGod to see how he takes pressure and gets altitude
  //He alsso uses the Az value to integrate and get vertical speed but uses a derivative filter.
  //float altitute = ((pow((SEA_PRESSURE / pressure), 1/5.257) - 1.0) * (temperature + 273.15)) / 0.0065;
  //#define SEA_PRESSURE     1013.25f
  //delP = -(mpl115a2.getPressure()-pressureKPA); ///If I climb in altitude getPressure will be smaller than pressureKPA. This means that delP will be positive as I climb in altitude
  //delZ = delP - ALTITUDE_COMMAND_PRESSURE; //So in order to be off the ground though I need to have delP be slightly positive otherwise I'll just crash straight into the ground
  //So turns out kyrell just uses height as the variable so we just need that.
  float temperature_kelvin = temperature + 273.15;
  float normalized_pressure = sea_pressure/pressure;
  altitude_raw_new = ((pow(normalized_pressure,0.19022) - 1.0) * temperature_kelvin) / 0.0065;
  if (altitude_raw_new < 200.0) {
    float alfa = 2*TAUHEIGHT/dT;
    altitude = (altitude_raw_new + altitude_raw_prev - altitude_prev*(1.0-alfa))/(alfa+1.0);
    altitude_prev = altitude;
    altitude_raw_prev = altitude_raw_new;
  }
}

void i2c::remove_anti_windup() {
  altitude_command = altitude;
  alt_int = 0; //Anti-windup - This only runs once now so it's ok
}

void i2c::print_I2C() {
  Serial.print("RPY=");
  Serial.print(roll);
  Serial.print(" ");
  
  Serial.print(pitch);
  Serial.print(" ");
  Serial.print(yaw);
  Serial.print(" ");
  
  Serial.print(roll_rate);
  Serial.print(" ");
  Serial.print(pitch_rate);
  Serial.print(" ");
  
  Serial.print(yaw_rate);
  Serial.print(" ");

  //Serial.print("dP=");
  //Serial.print(delP);
  //Serial.print(" ");
  
  //Serial.print("delZ=");
  //Serial.print(delZ);
  //Serial.print(" ");

  //Serial.print("alt_int=");
  //Serial.print(alt_int);
  //Serial.print(" ");

  //Serial.print("Az=");
  //Serial.print(Az);
}

void i2c::print_SD_I2C(File myFile) {
  myFile.print(roll);
  myFile.print(" ");
  myFile.print(pitch);
  myFile.print(" ");
  myFile.print(yaw);
  myFile.print(" ");
  myFile.print(roll_rate);
  myFile.print(" ");
  myFile.print(pitch_rate);
  myFile.print(" ");
  myFile.print(yaw_rate);
  myFile.print(" ");
  //myFile.print(pressure);
  //myFile.print(" ");
  //myFile.print(temperature);
  //myFile.print(" ");
  myFile.print(altitude_raw_prev);
  myFile.print(" ");
  myFile.print(altitude_prev);
  myFile.print(" ");
  myFile.print(altitude_command);
  myFile.print(" ");
}

