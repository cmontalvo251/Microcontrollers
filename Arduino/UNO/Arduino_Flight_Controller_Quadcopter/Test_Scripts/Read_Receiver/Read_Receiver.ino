void setup() {
  // put your setup code here, to run once:
  pinMode(10,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(pulseIn(10,HIGH));
}
