#define TX 18
#define RX 19

#define NUM_DIGITS 5

int led = 9;           // the PWM pin the LED is attached to
int place = pow(10,NUM_DIGITS-1);
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void setup() {

  //Here we initialize the LED
  pinMode(led, OUTPUT);
  analogWrite(led,brightness);
  
  Serial.begin(9600); //Talking to the computer
  Serial1.begin(115200); //receiving from another computer

  Serial.println("Child Arduino Working");
  delay(1000);

}

void loop() {
  while (Serial1.available() > 0) {
    //Grab 1 digit at a time
    char input = Serial1.read();
    
    Serial.print("Input = ");
    Serial.print(input);
    
    if (input == '\n') { //Read until you get to a line feed
      Serial.print("Input was a line feed");
      //Reset place
      place = pow(10,NUM_DIGITS-1);
      
      //Clear LED
      analogWrite(led,0);
      
      //Turn on led with the brightness that was read
      analogWrite(led,brightness);
      
      //Reset brightness
      brightness = 0;

      Serial1.write('\n');
    } else {
      //Convert the input to a number
      int digit = input - '0';
      
      Serial.print(" Digit = ");
      Serial.print(digit);
      Serial.print(" ");
      
      brightness += digit*place;
      
      Serial.print(" Number = ");
      Serial.print(brightness);
      Serial.print(" ");
      Serial.print(" Place = ");
      Serial.print(place);
      Serial.print('\n');
      
      place/=10;
    }
  }
} 
