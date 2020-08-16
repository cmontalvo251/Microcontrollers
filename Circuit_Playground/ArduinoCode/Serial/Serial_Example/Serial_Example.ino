int num_digits = 3;
// 255 = 2*100 + 5*10 + 5*1 = 2 * 10^2 + 5 * 10^1 + 5*10^0;
int place = pow(10,num_digits-1);
int number = 0;

#include <Adafruit_CircuitPlayground.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CircuitPlayground.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
    int inByte = Serial.read();
    //Serial.print(inByte);
    //Serial.print(" ");
    if (inByte == 10) {
      //Serial.print("Final Number Received = ");
      //Serial.print(number);
      for (int i = 0;i<=9;i++) {
        CircuitPlayground.setPixelColor(i,number,number,number);
      }
      //Serial.print('\n');
      number = 0;
      place = pow(10,num_digits-1);
    }
    int inNumber = inByte - 48;
    if (inNumber >= 0) {
      number += inNumber*place;
      place/=10;
      //Serial.print("Digit Recieved = ");
      //Serial.print(inNumber);
      //Serial.print(" ");
    }
  }
}
