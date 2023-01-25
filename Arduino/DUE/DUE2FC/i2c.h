#ifndef I2C_H
#define I2C_H

#include "userparams.h"
#include "sd.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
//Pressure and Temperature Sensor
#include <Adafruit_MPL115A2.h>

//Don't touch these
#define HEIGHTCONSTANT (4.0/HEIGHTSETTLE) 
#define TAUHEIGHT (1.0/HEIGHTCONSTANT)

class i2c {
  public:
    void read_I2C(float);
    void setup_I2C();
    void print_I2C();
    void print_SD_I2C(File);
    void remove_anti_windup();
    Adafruit_MPL115A2 mpl115a2;
    Adafruit_BNO055 bno;
    float lastI2CTime = 0;
    imu::Vector<3> euler;
    imu::Vector<3> rate_fc;
    imu::Vector<3> lin_accel;
    float sea_pressure = 0;
    float roll=0,pitch=0,yaw=0,roll_rate=0,pitch_rate=0,yaw_rate=0;
    float pressure = 0,temperature = 0;
    float delP = 0,delZ = 0;
    float altitude=0,altitude_raw_new=0,altitude_prev=0,altitude_raw_prev=0;
    float altitude_command = 0,alt_int = 0;
    i2c(); //constructor
};

#endif
