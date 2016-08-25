#include <Servo.h>

//Setup Sonar Sensor
#define frontTrigPin 6
#define frontEchoPin 7

//Setup Servo
Servo myservo;

//Make a float for later
float distance_to_box = 0;

void setup() {
  
  //Just need 9600 for prox sensors
  Serial.begin(9600);

  //Attach Servo
  myservo.attach(10);
  
  //Setup input and output
  SetupPin(frontTrigPin,frontEchoPin);
 
  Serial.println("Ready!");
}

void SetupPin(int TrigPin,int EchoPin) {
 pinMode(TrigPin,OUTPUT);
 pinMode(EchoPin,INPUT); 
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

void loop() {
  //Delay for kicks
  delay(30);

  //Get Distance to Box from Sonar Sensor
  float distance_to_box = Distance(frontTrigPin,frontEchoPin);
  
  //Debugging
  //distance_to_box++;

  //if (distance_to_box > 100) {
  //     distance_to_box = 0;
  //}

  //Distance from Box Command 
  float dcommand = 10;

  //Error Calculation
  float error = dcommand - distance_to_box;

  ///Decide what gain to use
  //distance_to_box = 100
  //error = -90
  //I want throttle to be full throttle
  //Full throttle = 180
  //Not moving = 90
  //So gain needs to be -1 but we will change this later

  //Gain 
  float kp = -0.1;

  //Compute Throttle command using proportional control
  float throttle_command = kp*error+94;

  //Send Throttle Command to Servo
  myservo.write(throttle_command);

  //Debug Servo
  //myservo.write(90);
  //delay(1000);
  //myservo.write(120);
  //delay(1000);

  //Print some stuffs
  Serial.print(millis());
  Serial.print(" ");
  Serial.print(distance_to_box);
  Serial.print(" ");
  Serial.println(throttle_command);
    
}


/* End code */

