//Declaring Variables
byte last_channel_steer, last_channel_thrust, last_channel_3, last_channel_4;
int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int counter_channel_1, counter_channel_2, counter_channel_3, counter_channel_4, start;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long zero_timer, timer_steer, timer_thrust, timer_3, timer_4, current_time;
int pulse_time_steer, pulse_time_thrust;
void setup() {
  // put your setup code here, to run once:
PCICR |= (1 << PCIE2);   // This is for pins 0-7
PCMSK2 |= (1 << PCINT19); //This is pin 3
PCMSK2 |= (1 << PCINT21); //This is pin 5
//PCICR |= (1 << PCIE0); //This is for pins 8-13
//PCMSK0 |= (1 << PCINT0); //This is pin 8
//PCMSK0 |= (1 << PCINT1); //This is pin 9

zero_timer = micros();
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print(pulse_time_thrust);
Serial.print(" ");
Serial.println(pulse_time_steer);
}

ISR(PCINT2_vect){ //This is for pins 0-7
//ISR(PCINT0_vect){ //This is for pins 8-13
  current_time = micros();
  if(PIND & B00001000){ //Port D is pins 0-7 - so this is pin 3
  //if (PINB & B00000001) { //Port B is pins 8-13 - so this is pin 8
    if(last_channel_thrust == 0)
    {
      last_channel_thrust = 1;
      timer_thrust = current_time;
    }
  }
  else if(last_channel_thrust == 1){
      last_channel_thrust = 0;
      pulse_time_thrust = current_time - timer_thrust;
  }
  if(PIND & B00100000){ //This is pin 5 from PortD
  //if (PINB & B00000010){ //This is pin 9 from PortB
    if(last_channel_steer == 0)
    {
      last_channel_steer = 1;
      timer_steer = current_time;
    }
  }
  else if(last_channel_steer == 1){
      last_channel_steer = 0;
      pulse_time_steer = current_time - timer_steer;
    
  }  
}

