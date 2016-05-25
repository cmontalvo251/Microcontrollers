int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
char val;         // incoming serial word
int ledPin = 13;
boolean ledState = LOW;

void setup()
{
  pinMode(ledPin,OUTPUT);
  // start serial port at 9600 bps:
  Serial.begin(9600);
  establishContact();  // send a byte to establish contact until receiver responds 
}

void loop()
{
  //Infinite loop until we get something
  while(Serial.available()==0);
  
  String message = Serial.readString();
  if (message == "test")
  {
    firstSensor = random(1,10);
    secondSensor = random(1,10);
    thirdSensor = random(1,10); 
    // send sensor values:
    Serial.write(firstSensor); 
    Serial.write(secondSensor);
    Serial.write(thirdSensor);     
  }
  else 
  {
    Serial.flush();
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("A");   // send a capital A
    delay(300);
  }
  val = Serial.read();
}
