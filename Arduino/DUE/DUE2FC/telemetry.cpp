#include "telemetry.h"

Telemetry::Telemetry() {

}

void Telemetry::begin(HardwareSerial *SerialIn) {
  // start serial port at 9600 bps:
  mySerial = SerialIn;
  mySerial->begin(9600);
}

void Telemetry::serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    
    mySerial->print(c1); //does this output a command between prints?
    mySerial->print(c2); //otherwise line 75 on .pde won't work because their aren't any commas (Look up like 10 lines of code)
  }
}

void Telemetry::serialPrintFloatArr(float * arr, int length) {
  for(int i=0; i<length; i++) {
    serialFloatPrint(arr[i]);
    mySerial->print(","); //The commas are here dude. 
  }
}

void Telemetry::write(float time_now) {
  if (mySerial->available()) { 
    Serial.print("Message Received Sending output ! \n");
    receive_message = mySerial->readStringUntil('\n');
    last_read = time_now;
    serialPrintFloatArr(serialoutarray,XBEE_VARS);
    mySerial->println("\r\n");
    mySerial->flush(); //wait for outgoing data to go out
  }
}
