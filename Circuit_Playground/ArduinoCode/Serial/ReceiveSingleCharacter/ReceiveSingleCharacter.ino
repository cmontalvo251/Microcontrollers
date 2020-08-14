#include <Adafruit_CircuitPlayground.h>

void setup() {
  //Turn on Serial
  Serial.begin(9600);
  //kick off CPX
  CircuitPlayground.begin();
}

void loop() {
  //Constantly check for incoming characters
  while (Serial.available() > 0) {
    char input = Serial.read();
    //Convert the input to a number
    int number = input - '0';
    //Then turn on that neopixel
    if ((number >= 0) && (number <= 10)) {
      CircuitPlayground.clearPixels();
      CircuitPlayground.setPixelColor(number,100,100,100);
    }
  }
}
