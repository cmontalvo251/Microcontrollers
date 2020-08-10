char s = '3';

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  //Make dummy data
  Serial.print(s);
  //Send data to processing
  Serial.print("\r\n");
  Serial.flush(); //wait for outgoing data to go out
  delay(1000);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN,LOW);
}
