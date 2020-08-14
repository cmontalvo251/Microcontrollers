int number = 250;
int num_digits = 3;
char output_chars[3];
String output;

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}

void clearDigits() {
  for(int ix = 0;ix<num_digits;ix++) {
    output_chars[ix] = '0';
  }
}

void string2chars() {
  clearDigits();
  int j = num_digits-1;
  for (int i=output.length()-1;i>=0;i--) {
    output_chars[j] = char(output[i]);
    j--;
  }
}

void printchars() {
  for(int id=0;id<num_digits;id++) {
    Serial.print(output_chars[id]);
    //Serial.print(".");
  }
}

void loop()
{
  //Convert the number to a string of chars
  output = String(number);
  //Send via serial
  //Serial.print(number);
  //Serial.print(" ");
  //Serial.print(output);
  //Serial.print(" ");
  //Serial.print(output.length());
  //Serial.print(" ");
  //Serial.print(output[0]);
  //Pad output_chars with '0s'
  string2chars();
  //Serial.print(" ");
  printchars();
  //Send data to processing
  Serial.print("\r\n");
  Serial.flush(); //wait for outgoing data to go out
  delay(1000);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN,LOW);
  //Increment the number by 1
  number+=1;
  if (number > 255) {
    number = 0;
  }
}
