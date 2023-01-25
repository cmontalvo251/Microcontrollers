#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "userparams.h"
#include <Arduino.h>

class Telemetry {
  public:
    String receive_message;
    float serialoutarray[XBEE_VARS];
    float last_read = 0;
    float deltaTime;
    void serialFloatPrint(float);
    void serialPrintFloatArr(float*, int);
    Telemetry(); //constructor
    void begin(HardwareSerial*); 
    HardwareSerial* mySerial;
    void write(float);
};

#endif TELEMETRY_H
