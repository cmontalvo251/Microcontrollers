#include <Adafruit_CircuitPlayground.h>

int num_digits = 3;
int place = pow(10,num_digits-1);
char output_chars[3];
String output;
int number = 0;

void setup() {
  //Turn on Serial
  Serial.begin(9600);
  //kick off CPX
  CircuitPlayground.begin();
  clearDigits();
}


void string2chars() {
  clearDigits();
  int j = num_digits-1;
  for (int i=output.length()-1;i>=0;i--) {
    output_chars[j] = char(output[i]);
    j--;
  }
}

void clearDigits() {
  for(int ix = 0;ix<num_digits;ix++) {
    output_chars[ix] = '0';
  }
}

void printchars() {
  for(int id=0;id<num_digits;id++) {
    Serial.print(output_chars[id]);
    //Serial.print(".");
  }
  Serial.print('\n'); //Processing is expecting a newline
}

void loop() {
  //Constantly check for incoming characters
  while (Serial.available() > 0) {
    //Grab 1 digit at a time
    char input = Serial.read();
    //Serial.print("Input = ");
    //Serial.print(input);
    if (input == '\n') { //Read until you get to a line feed
      //Serial.print("Input was a line feed");
      //Reset place
      place = pow(10,num_digits-1);
      //Clear pixels
      CircuitPlayground.clearPixels();
      //Turn on neopixel with that brightness
      CircuitPlayground.setPixelColor(0,number,number,number);
      //Then multiply the number by 2
      int out_number = 2*number;
      if (out_number > 255) {
        out_number = 255; //have to truncate at some point
      }
      //Then send the out_number back to processing
      //Serial.print(" Number = ");
      //Serial.print(number);
      output = String(out_number);
      //Serial.print(" String = ");
      //Serial.print(output);
      //Serial.print('\n');
      string2chars();
      printchars();
      //Reset number
      number = 0;
    } else {
      //Convert the input to a number
      int digit = input - '0';
      //Serial.print(" Digit = ");
      //Serial.print(digit);
      //Serial.print(" ");
      number += digit*place;
      //Serial.print(" Number = ");
      //Serial.print(number);
      //Serial.print(" ");
      //Serial.print(" Place = ");
      //Serial.print(place);
      //Serial.print('\n');
      place/=10;
    }
  }
}
