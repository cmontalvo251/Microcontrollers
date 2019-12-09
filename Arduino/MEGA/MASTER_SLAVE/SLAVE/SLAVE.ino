#include <Servo.h>

char input_chars[6];

Servo myservo;

void setup() {
  //Setup a dummy servo for debugging
  myservo.attach(7);
  //Setup serial communication with computer
  Serial.begin(9600);
  //Setup communication with other arduino
  Serial1.begin(115200);
  //Send a pulse over to other arduino
  Serial1.println('1');
}

void loop() {
  //Check for data from other arduino
  if (Serial1.available()) {
    Serial.println("Data received from other arduino");
    int ctr = 0;
    for (int i = 0;i<6;i++) {
      input_chars[i] = ' ';
    }
    while (Serial1.available()) {
      input_chars[ctr] = Serial1.read();
      ctr++;
      delay(0.5);
    }
    Serial.print("Raw characters = ");
    Serial.println(input_chars);
    //Convert number to string
    String received_string = String(input_chars);
    //Then to an int
    int servo_command = int(received_string.toFloat());
    Serial.print("Command = ");
    Serial.println(servo_command);
    Serial1.flush();
    while(Serial1.available()) {
      char d = Serial1.read();
    }
    myservo.write(servo_command);
    //Notify other arduino that all is good
    Serial1.println('1');
    Serial1.flush();
  }
}
