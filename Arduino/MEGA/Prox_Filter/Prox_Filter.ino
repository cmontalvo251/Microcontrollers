
#define NUM_CHARS 4

char input_chars[NUM_CHARS];
boolean oktosend = true;

#define onlyTrigPin 9
#define onlyEchoPin 8

void SetupPin(int TrigPin, int EchoPin) {
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}

long Distance(int TrigPin, int EchoPin) {
  long duration;
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  duration = pulseIn(EchoPin, HIGH);
  long cm = (duration / 2.0) / 29.1;
  if (cm > 180) {
    cm = 180;
  }
  return cm;
}

void setup() {
  Serial.begin(9600);
  SetupPin(onlyTrigPin, onlyEchoPin);
  Serial.println("Setup complete");
}

double cm_old,cm;

void loop() {

  ///////////?READ PROX//////////////////
  double cm = Distance(onlyTrigPin, onlyEchoPin);
  Serial.print("New = ");
  Serial.print(cm);

  //Filter Signal
  double s = 0.9;
  cm = (1-s)*cm + s*cm_old;
  cm_old = cm;

  Serial.print("Filtered = ");
  Serial.print(cm);
  Serial.println();
  
}
