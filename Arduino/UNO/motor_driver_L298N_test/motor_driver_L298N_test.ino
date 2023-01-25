/*  Arduino DC Motor Control - PWM | H-Bridge | L298N
         Example 02 - Arduino Robot Car Control
    by Dejan Nedelkovski, www.HowToMechatronics.com
*/

//#define enA 9
//#define in1 4
//#define in2 5
#define enB 10
#define in3 6
#define in4 7

//int motorSpeedA = 0;
int motorSpeedB = 0;

void setup() {
  //pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  //pinMode(in1, OUTPUT);
  //pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Backward....");

  // Set Motor B backward
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // Convert the declining Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed
  motorSpeedB = 255;
  analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
  delay(5000.0);

  Serial.println("Forward....");
  
  // Set Motor B forward
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  motorSpeedB = 255;
  analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
  delay(5000.0);

  Serial.println("Stop!!!....");

  //Set Motor Zero
  motorSpeedB = 0;
  analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
  delay(5000.0);
}

