#include <Adafruit_CircuitPlayground.h>

int num_digits = 3;
int place = (num_digits-1)*10;
int number = 0;

void setup() {
  //Turn on Serial
  Serial.begin(9600);
  //kick off CPX
  CircuitPlayground.begin();
}

void loop() {
  //Constantly check for incoming characters
  while (Serial.available() > 0) {
    //Grab 1 digit at a time
    char input = Serial.read();
    if (input == '\n') { //Read until you get to a line feed
      //Reset place
      place = (num_digits-1)*10;
      //Clear pixels
      CircuitPlayground.clearPixels();
      //Turn on neopixel with that brightness
      CircuitPlayground.setPixelColor(0,number,number,number);
      //Reset number
      number = 0;
    } else {
      //Convert the input to a number
      int digit = input - '0';
      number += digit*place;
      place/=10;
    }
  }
}
