int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
char val;         // incoming serial word
int inByte;
int ledPin = 13;
boolean ledState = LOW;
int numVars = 3;
int serialinarray[3];
int serialcount = 0;

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
  if (Serial.available() > 2) {
    for (int idx = 0;idx<3;idx++) {
      serialinarray[serialcount] = Serial.read();
      serialcount++;
    }
    firstSensor = serialinarray[0]*2;
    secondSensor = serialinarray[1]*3;
    thirdSensor = serialinarray[2]*4;
    serialcount = 0;
    // send sensor values:
    Serial.write(firstSensor); 
    Serial.write(secondSensor);
    Serial.write(thirdSensor);
    Serial.flush(); //wait for outgoing data to go out
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
  while (Serial.available() > 0) {
    val = Serial.read(); //This will read any dummy characters left over
  }
}
