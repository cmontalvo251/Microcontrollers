#define TX 18
#define RX 19

#define NUM_DIGITS 3

#include <Servo.h>

int servoPin = 7;          // the PWM pin the LED is attached to
int place = pow(10,NUM_DIGITS-1);
int angle = 0;

Servo myservo;

void setup() {

  myservo.attach(servoPin);
  
  Serial.begin(9600); //Talking to the computer
  Serial1.begin(115200); //receiving from another computer

  Serial.println("Child Arduino Working");
  delay(1000);

}

void loop() {

  //Debug
  //angle+=20;
  //if (angle>180){angle=0;}
  //myservo.write(angle);
  //delay(1000);

  //while (false) {
  while (Serial1.available() > 0) {
    //Grab 1 digit at a time
    char input = Serial1.read();
    Serial.print("Input = ");
    Serial.print(input);
    
    if (input == '\n') { //Read until you get to a line feed
      Serial.print("Input was a line feed");
      //Reset place
      place = pow(10,NUM_DIGITS-1);
      
      //Clear LED
      myservo.write(angle);
      
      //Reset angle
      angle = 0;

      Serial1.write('\n');
    } else {
      //Convert the input to a number
      int digit = input - '0';
      
      Serial.print(" Digit = ");
      Serial.print(digit);
      Serial.print(" ");
      
      angle += digit*place;
      
      Serial.print(" Number = ");
      Serial.print(angle);
      Serial.print(" ");
      Serial.print(" Place = ");
      Serial.print(place);
      Serial.print('\n');
      
      place/=10;
    }
  }
} 
