extern void DoHardwareInLoop(void);

//#include <Adafruit_CircuitPlayground.h>

void blinktwice(int d) {
  digitalWrite(LED_BUILTIN,HIGH);
  delay(d);
  digitalWrite(LED_BUILTIN,LOW);
  delay(d);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(d);
  digitalWrite(LED_BUILTIN,LOW);
  delay(d);
  digitalWrite(LED_BUILTIN,HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Turn on the Builtin LED
  pinMode(LED_BUILTIN,OUTPUT);
  //Initialize the CPX
  //CircuitPlayground.begin();
  //Blink once to notify that the arduino is ready
  blinktwice(1000);
  blinktwice(100);
}

void loop() {

  //ClearPixels
  //CircuitPlayground.clearPixels();
  
  char c;

  //Ok so this code here basically waits until the arduino
  //read a 'w'. 
  do {
    c=Serial.read();
  } while (c != 'w');
  //In order to notify the user that the arduino received a w
  //and a new line below.
  //Set the first Neopixel to red
  //CircuitPlayground.setPixelColor(0,255,0,0);
  do {
    c=Serial.read();
  } while (c != '\r');
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

  //I'll then have it set the 3rd Neopixel to red
  //CircuitPlayground.setPixelColor(2,255,0,0);
  //delay(1000); //just for debugging

  //After that it will run the DoHIL routine
  DoHardwareInLoop();

}
