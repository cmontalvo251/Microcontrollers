extern void SendNumbers(void);

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
  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {  
  char c;

  digitalWrite(LED_BUILTIN,LOW);

  //Ok so this code here basically waits until the arduino
  //read a 'w'. 
  do {
    c=Serial.read();
  } while (c != 'w');
  //In order to notify the user that the arduino received a w
  //and a new line below.

  //Serial.print('q');
  //Set the first Neopixel to red
  //CircuitPlayground.setPixelColor(0,255,0,0);
  do {
    c=Serial.read();
  } while (c != '\r');

  digitalWrite(LED_BUILTIN,HIGH);

  //Serial.print('r');
  //Then set the second Neopixel to red
  //CircuitPlayground.setPixelColor(1,255,0,0);

  //Once that happens the Arduino is going to return 'w\r\n'
  //to whatever is listening
  //This also returns w\r\n to the arduino
  c = 'w'; //ASCII CODE = 119
  Serial.print(c);
  c = '\r'; //ASCII CODE = 13
  Serial.print(c);
  c = '\n'; //ASCII CODE = 10
  Serial.print(c); 
  
  //After that it will run the DoHIL routine
  SendNumbers();

}
