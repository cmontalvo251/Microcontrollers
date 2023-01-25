#ifndef GPS_H
#define GPS_H

#include "userparams.h"
#include "sd.h"
#include <Adafruit_GPS.h>
#include <Arduino.h>

//static Adafruit_GPS GPS(&Serial1);

class Montalvo_GPS {
  public:
    Adafruit_GPS *GPS;
    void setup_GPS();
    void poll_GPS();  
    void print_GPS();
    void print_SD_GPS(File);
    Montalvo_GPS(); //Constructor
};

#endif GPS_H
