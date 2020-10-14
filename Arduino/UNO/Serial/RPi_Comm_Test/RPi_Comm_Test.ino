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
  char c;

  //Ok so this code here basically waits until the arduino
  //read a 'w'. 
  if (Serial.available()) {
    c=Serial.read();
    int d = int(c);
    int dout = d + 2;
    char cout = char(dout);
    Serial.write(cout);
  }
}
