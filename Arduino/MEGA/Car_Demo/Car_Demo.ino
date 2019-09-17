//Setup Sonar Sensor
#define frontTrigPin 31
#define frontEchoPin 33

#include <Servo.h>
Servo myservo;

float nextTime = 0;
float startTime = 0;

void setup() {
  // put your setup code here, to run once:
 //Just need 9600 for prox sensors
  Serial.begin(9600);

  //Setup input and output
  SetupPin(frontTrigPin,frontEchoPin);

  //Attach Servo
  myservo.attach(9);

  Serial.println("Ready!");

  nextTime = millis()/1000.0;
  startTime = millis()/1000.0;
}

long Distance(int TrigPin,int EchoPin) {
  long duration; 
  digitalWrite(TrigPin,LOW);
  delayMicroseconds(2); 
  digitalWrite(TrigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin,LOW);
  duration = pulseIn(EchoPin,HIGH);
  long cm = (duration/2.0)/29.1;  
  if (cm > 100) {
    cm = 100;
  }
  return cm;
}  

void SetupPin(int TrigPin,int EchoPin) {
 pinMode(TrigPin,OUTPUT);
 pinMode(EchoPin,INPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:

  //Get Sonar Reading
  float d = Distance(frontTrigPin,frontEchoPin);

  //Control Law
  float dc = 15; //sensor can't actually read zero. You also don't have brakes so you need to coast for a while

  float error = dc - d;

  //Proportional Control
  float kp = -0.4; //Start small. Don't break shit. Unless you want to.

  //PWM Signal
  float u = kp*error + 1525; //is 1525 the offset for not moving forward? Perhaps we need to add some integral gain to determine the min signal

  //Complete the loop
  if ((millis()/1000.0 - startTime) > 6) {
    myservo.writeMicroseconds(u);
  } else {
    myservo.writeMicroseconds(1500); //Is this really the initialization value?
  }
  

  if (millis()/1000.0 > nextTime + 0.05) {
    nextTime = millis()/1000.0;
    Serial.print(millis()/1000.0);
    Serial.print(" ");
    Serial.print(d);
    Serial.print(" ");
    Serial.print(error);
    Serial.print(" ");
    Serial.print(u);
    Serial.println();
  }

  

}
