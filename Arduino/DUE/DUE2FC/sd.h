#ifndef SD_H
#define SD_H

#include <SD.h>

class sdcard {
  public:
    void setup_SD();
    sdcard(); //constructor
    File myFile;
};

#endif SD_H
