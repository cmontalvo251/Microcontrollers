// Set the pins used
#define ledPin 13

#define frontTrigPin 46
#define frontEchoPin 47

#define leftTrigPin 48
#define leftEchoPin 49

#define rightTrigPin 44
#define rightEchoPin 45

long cmright,cmleft,cmfront;

void setup() {
  
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  
  //Setup input and output
  SetupPin(leftTrigPin,leftEchoPin);
  SetupPin(frontTrigPin,frontEchoPin);
  SetupPin(rightTrigPin,rightEchoPin);
  
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
  //Here is where we will put the Prox Code
  //Writing just the current time
  delay(100); 

  cmright = Distance(rightTrigPin,rightEchoPin);
  cmleft = Distance(leftTrigPin,leftEchoPin);
  cmfront = Distance(frontTrigPin,frontEchoPin);  

  Serial.print(millis());
  Serial.print(" ");
  Serial.print(cmleft);
  Serial.print(" ");
  Serial.print(cmfront);
  Serial.print(" ");
  Serial.print(cmright);
  Serial.print("\n"); 
  
}


/* End code */

