#include <Servo.h>

char input_chars[6];

Servo myservo;

void setup() {
  //Setup a dummy servo for debugging
  myservo.attach(7);
  //Setup serial communication with computer
  Serial.begin(9600);
  //Setup communication with other arduino
  Serial1.begin(9600);
  //Send a pulse over to other arduino
  Serial1.println('1');
}

void loop() {
  //Check for data from other arduino
  if (Serial1.available()) {
    Serial.println("Data received from other arduino");
    int ctr = 0;
    while (Serial1.available()) {
      input_chars[ctr] = Serial1.read();
      ctr++;
    }
    //Convert number to string
    String received_string = String(input_chars);
    //Then to an int
    int servo_command = int(received_string.toFloat());
    Serial.print("Command = ");
    Serial.print(servo_command);
    myservo.write(servo_command);
    //Notify other arduino that all is good
    Serial1.println('1');
  }
}
