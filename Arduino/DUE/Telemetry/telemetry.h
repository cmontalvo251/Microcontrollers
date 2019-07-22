#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "userparams.h"
#include <Arduino.h>

class Telemetry {
  public:
    int numVars = 4;
    String receive_message;
    float serialoutarray[4];
    float last_read = 0;
    float deltaTime;
    void serialFloatPrint(float);
    void serialPrintFloatArr(float*, int);
    Telemetry(HardwareSerial*); //constructor
    HardwareSerial *mySerial;
    void write(float);
};

#endif TELEMETRY_H
