#include <Servo.h>

// Create Servo objects for each control surface and ESC
Servo aileronServo;
Servo rudderServo;
Servo elevatorServo;
Servo esc;  // ESC as a Servo object

// Define Limits of Servos
const int PULSEMIN = 500;
const int PULSEMAX = 3000;

// Define the pins for the receiver signals and the servo/ESC control pins
const int aileronPin = 9;         // Aileron signal from the receiver
const int rudderPin = 11;         // Rudder signal from the receiver
const int elevatorPin = 12;       // Elevator signal from the receiver
const int throttlePin = 10;       // Throttle signal from the receiver
const int auxOnePin = 8;          // Aux 1 signal (ch 5) from reciever

const int aileronServoPin = 2;    // Pin for controlling the aileron servo
const int rudderServoPin = 4;     // Pin for controlling the rudder servo
const int elevatorServoPin = 5;   // Pin for controlling the elevator servo
const int escPin = 6;             // Pin for controlling the ESC

// Variables to store pulse widths and servo positions
unsigned long pulseWidthAileron;
unsigned long pulseWidthRudder;
unsigned long pulseWidthElevator;
unsigned long pulseWidthThrottle; // For ESC control
unsigned long pulseWidthAuxOne;   // For Aux 1 - Ch 5

void setup() {
  // Attach servos and ESC to corresponding pins
  aileronServo.attach(aileronServoPin);
  rudderServo.attach(rudderServoPin);
  elevatorServo.attach(elevatorServoPin);
  esc.attach(escPin);

  // Set up the receiver pins as input
  pinMode(aileronPin, INPUT);
  pinMode(rudderPin, INPUT);
  pinMode(elevatorPin, INPUT);
  pinMode(throttlePin, INPUT);
  pinMode(auxOnePin, INPUT);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read pulse widths from the receiver channels
  int pulseWidthAileronIN = pulseIn(aileronPin, HIGH);
  int pulseWidthRudderIN = pulseIn(rudderPin, HIGH);
  int pulseWidthElevatorIN = pulseIn(elevatorPin, HIGH);
  int pulseWidthThrottleIN = pulseIn(throttlePin, HIGH)-200; // Read throttle signal
  int pulseWidthAuxOneIN = pulseIn(auxOnePin, HIGH)

  float s = 0.75;
  pulseWidthAileron = pulseWidthAileron*s + pulseWidthAileronIN*(1-s);
  pulseWidthRudder = pulseWidthRudder*s + pulseWidthRudderIN*(1-s);
  pulseWidthElevator = pulseWidthElevator*s + pulseWidthElevatorIN*(1-s);
  pulseWidthThrottle = pulseWidthThrottle*s + pulseWidthThrottleIN*(1-s);
  pulseWidthAuxOne = pulseWidthAuxOne*s + pulseWidthAuxOne*(1-s);

  // Set servo and ESC positions
  if (pulseWidthAileron < PULSEMIN) {
    pulseWidthAileron = PULSEMIN;
  } 
  if (pulseWidthAileron > PULSEMAX) {
    pulseWidthAileron = PULSEMAX;
  }
  if (pulseWidthElevator < PULSEMIN) {
    pulseWidthElevator = PULSEMIN;
  } 
  if (pulseWidthElevator > PULSEMAX) {
    pulseWidthElevator = PULSEMAX;
  }
    if (pulseWidthRudder < PULSEMIN) {
    pulseWidthRudder = PULSEMIN;
  } 
  if (pulseWidthRudder > PULSEMAX) {
    pulseWidthRudder = PULSEMAX;
  }
    if (pulseWidthThrottle < PULSEMIN) {
    pulseWidthThrottle = PULSEMIN;
  } 
  if (pulseWidthThrottle > PULSEMAX) {
    pulseWidthThrottle = PULSEMAX;
  }
  aileronServo.writeMicroseconds(pulseWidthAileron);
  rudderServo.writeMicroseconds(pulseWidthRudder);
  elevatorServo.writeMicroseconds(pulseWidthElevator);
  esc.writeMicroseconds(pulseWidthThrottle); // Control ESC (motor speed) with throttle signal

  // Debugging output
  Serial.print("tTAER: ");
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.print(pulseWidthThrottle);
  Serial.print(" ");
  Serial.print(pulseWidthAileron);
  Serial.print(" ");
  Serial.print(pulseWidthElevator);
  Serial.print(" ");
  Serial.print(pulseWidthRudder);
  Serial.println("");
  Serial.print(pulseWidthAuxOne);
  Serial.println("");

  // Delay for a short time to allow servo movement
  delay(1);
}



//Tactic channels:
//ch 1 - aileron
//ch 2 - elevator
//ch 3 - throttle
//ch 4 - rudder
//ch 5 - aux 1
//ch 6 - aux 2