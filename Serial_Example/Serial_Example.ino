void setup() 
{
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
}


void loop()
{
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  //send 'Hello, world!' over the serial port
  Serial.println(random(1,10));
  //wait 100 milliseconds so we don't drive ourselves crazy
  delay(100);
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
}

