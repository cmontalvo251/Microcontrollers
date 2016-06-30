//Declaring Variables
byte last_channel_steer, last_channel_thrust, last_channel_3, last_channel_4;
int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int counter_channel_1, counter_channel_2, counter_channel_3, counter_channel_4, start;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long zero_timer, timer_steer, timer_thrust, timer_3, timer_4, current_time;
int pulse_time_steer, pulse_time_thrust;
void setup() {
  // put your setup code here, to run once:
PCICR |= (1 << PCIE2);  
PCMSK2 |= (1 << PCINT19);
PCMSK2 |= (1 << PCINT21);
//PCICR |= (1 << PCIE0);
//PCMSK0 |= (1 << PCINT0);
//PCMSK0 |= (1 << PCINT1);

zero_timer = micros();
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print(pulse_time_thrust);
Serial.print(" ");
Serial.println(pulse_time_steer);
}

ISR(PCINT2_vect){
//ISR(PCINT0_vect){
  current_time = micros();
  if(PIND & B00001000){
  //if (PINB & B00000001) {
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
  if(PIND & B00100000){
  //if (PINB & B00000010){
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

