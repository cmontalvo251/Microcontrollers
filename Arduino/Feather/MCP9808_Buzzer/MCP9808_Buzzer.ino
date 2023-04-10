//STEP 1 - Download Arduino IDE
//STEP 2 - Add https://adafruit.github.io/arduino-board-index/package_adafruit_index.json to Additional boards manager URLS in Preferences
//STEP 3 - Go to board manager and look for 32u4
//STEP 4 - Go to manage libraries and look for MCP9808 and Streaming
//STEP 5 - Try and compile the code. 
//STEP 6 - Upload and open Serial monitor at 115200


const int buzzerPin = 11;

#include <MCP9808.h>      // http://github.com/JChristensen/MCP9808
#include <Streaming.h>    // http://arduiniana.org/libraries/streaming/

MCP9808 mySensor;
uint8_t status;

void setup() {
  Serial.begin(115200);
  Serial.println("Hey oh here we go.....");
  // put your setup code here, to run once:
  pinMode(buzzerPin,OUTPUT);

  delay(1000);                        // time for the sensor to perform an initial temperature conversion
  status = mySensor.begin();  // initialize the hardware
  if ( status != 0 )
  {
    Serial << "Error reading sensor, status=" << status << endl;
    Serial.flush();
    while (1);                      // loop until reset
  }
  mySensor.tUpper = 25 * 4;           // 25C (77F)
  mySensor.tLower = -10 * 4;          // -10C (14F)
  mySensor.tCritical = 30 * 4;        // 30C (86F)
  mySensor.write();                   // write the changed values
}

void loop() {
  float C;
  if ( (status = mySensor.read()) == 0 ) {
    C = mySensor.tAmbient / 16.0;
    float F = C * 9.0 / 5.0 + 32.0;
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(C);
    Serial.print("C \n");
  }
  if (C > 23) {
    analogWrite(buzzerPin,27);
    delay(500);
  } else {
    analogWrite(buzzerPin,0);
    delay(100);    
  }
}
