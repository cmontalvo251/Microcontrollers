#include "HardwareSerial.h"
#include "Arduino.h"

union inparser {
  long inversion;
  float floatversion;
};

void SendNumbers(){
  union inparser inputvar;
  //For now let's just send back mag+gyro as a debug state
  inputvar.floatversion = millis()/1000.0;
  char outline[20];
  sprintf(outline, "H:%08lx \0",inputvar.inversion);
  Serial.print(outline);
  Serial.print('\r');
  inputvar.floatversion = 10.0*rand();
  sprintf(outline, "H:%08lx \0",inputvar.inversion);
  Serial.print(outline);
  Serial.print('\r');
  /////////////////////////////////////////////////// */

}
