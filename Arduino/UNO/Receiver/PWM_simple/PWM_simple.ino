//Setup routine

#define RECEIVER_INPUT_PIN 7

void setup(){
  Serial.begin(9600);
}

//Main program loop
void loop(){
  int receiver_input_channel_1 = pulseIn(RECEIVER_INPUT_PIN,HIGH);
  Serial.println(receiver_input_channel_1);
}
