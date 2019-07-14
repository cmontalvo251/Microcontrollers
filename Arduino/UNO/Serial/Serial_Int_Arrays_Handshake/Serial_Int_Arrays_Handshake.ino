//int firstSensor = 0;    // first analog sensor
//int secondSensor = 0;   // second analog sensor
//int thirdSensor = 0;    // digital sensor
//char val;         // incoming serial word
//int inByte;
//int ledPin = 13;
//boolean ledState = LOW;
int numVars = 3;
int serialinarray[3];
int serialcount = 0;

void setup()
{
  //pinMode(ledPin,OUTPUT);
  // start serial port at 9600 bps:
  Serial.begin(9600);
  //establishContact();  // send a byte to establish contact until receiver responds 
}

void loop()
{
  //Only read if you've got adequate data
  //No need to perform a serialEvent because we don't need multiple processes here. We
  //literally just need to perform some calculations on the Arduino and send it back.
  if (Serial.available() > 2) { //I think 2 means 2 integers either that or an int is actually 1 byte
    for (int idx = 0;idx<3;idx++) {
      serialinarray[idx] = Serial.read();
      Serial.write(serialinarray[idx]);
    }
    Serial.flush(); //wait for outgoing data to go out
  }
}

/* void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    Serial.flush(); //wait for outgoing data
    //delay(300);
  }
  while (Serial.available() > 0) {
    val = Serial.read(); //This will read any dummy characters left over
  }
} */
