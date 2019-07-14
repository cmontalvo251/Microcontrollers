//Arduino Code: (transfer this program to the Arduino)
//Developed by Rajarshi Roy
//Edited by Carlos Montalvo
int red, green, blue; //red, green and blue values
int RedPin = 9; //Red pin 9 has a PWM
int GreenPin = 10; //Green pin 10 has a PWM
int BluePin = 11; //Blue pin 11 has a PWM

void setup()
{

  Serial.begin(9600);
  //initial values (no significance)
  int red = 255;
  int blue = 255;
  int green = 255;
  
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
}

void loop()
{
  digitalWrite(13,LOW);

  //protocol expects data in format of 4 bytes
  //(xff) as a marker to ensure proper synchronization always
  //followed by red, green, blue bytes
  if (Serial.available()>=4) {
    if(Serial.read() == 0xff){
      red = Serial.read();
      green= Serial.read();
      blue = Serial.read();
      digitalWrite(13, HIGH);
    }
  }
  
  //finally control led brightness through pulse-width modulation
  analogWrite (RedPin, red);
  analogWrite (GreenPin, green);
  analogWrite (BluePin, blue);
  delay(100); //just to be safe
  
}

