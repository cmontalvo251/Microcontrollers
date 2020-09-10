extern void SendReceiveNumber(void);

void blinktwice(int d) {
  digitalWrite(LED_BUILTIN,HIGH);
  delay(d);
  digitalWrite(LED_BUILTIN,LOW);
  delay(d);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(d);
  digitalWrite(LED_BUILTIN,LOW);
  delay(d);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Turn on the Builtin LED
  pinMode(LED_BUILTIN,OUTPUT);
  //Blink once to notify that the arduino is ready
  //blinktwice(1000);
  blinktwice(100);
}

void loop() {  
  char inchar = '\0';
  do {
    inchar = Serial.read();
  } while (inchar != 'w');
  
  //After that it will run the DoHIL routine
  SendReceiveNumber();
  delay(1000);

}
