int i;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  char filename[15];
  strcpy(filename, "QUAD000.TXT");
  i++;
  if (i > 999) {
    i = 0;
  }
  filename[4] = '0' + i/100;
  filename[5] = '0' + (i/10)%10;
  filename[6] = '0' + i%10;
  
  Serial.println(filename);
  delay(10);
}
