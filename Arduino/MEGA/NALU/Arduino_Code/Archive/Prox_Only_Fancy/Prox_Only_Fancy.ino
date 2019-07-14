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

  //Set up masks for PCINT pins? Do I need this?
  //PCICR |= (1 << PCIE0);

}

void SetupPin(int TrigPin,int EchoPin) {
 pinMode(TrigPin,OUTPUT);
 //pinMode(EchoPin,INPUT);  No need because all pins default to input
}

void loop() {

  long data[3];
  
  //Delay so we don't get a ton of output
  delay(100); 

  //Left Sensor
  PORTL &= B11111101; //Left Trig Pin - 48 (PORTL byte 1) (PORTD byte 5)
  delayMicroseconds(2); 
  PORTL |= B00000010;
  delayMicroseconds(10);
  PORTL &= B11111101;
  data[0] = pulseIn(leftEchoPin,HIGH);

  delay(2);

  //Front Sensor
  PORTL &= B11110111; //Front Trig Pin - 46 (PORTL byte 3) (PORTD byte 3)
  delayMicroseconds(2); 
  PORTL |= B00001000;
  delayMicroseconds(10);
  PORTL &= B11110111;
  data[1] = pulseIn(frontEchoPin,HIGH);

  delay(2);

  //Right Sensor
  PORTL &= B11011111; //Right Trig Pin - 44 (PORTL byte 5) (PORTD byte 1)
  delayMicroseconds(2); 
  PORTL |= B00100000;
  delayMicroseconds(10);
  PORTL &= B11011111;
  data[2] = pulseIn(rightEchoPin,HIGH);

  //Convert Duration to Distance
  for (int idx = 0;idx<3;idx++){
    data[idx] = (data[idx]/2.0)/29.1;  
    if (data[idx] > 100) {
      data[idx] = 0;
    }
  }

  Serial.print(millis());
  for (int idx = 0;idx<3;idx++) {
    Serial.print(" ");
    Serial.print(data[idx]);
  }
  Serial.print("\n"); 
  
}


/* End code */

