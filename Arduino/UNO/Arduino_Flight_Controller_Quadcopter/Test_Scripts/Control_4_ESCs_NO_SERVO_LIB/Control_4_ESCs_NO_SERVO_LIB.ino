unsigned long timer_esc[4];

byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer, loop_timer, esc_loop_timer;

void setup() {
  Serial.begin(9600);

  //Attach ESCs
  SetupESCs();

  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  zero_timer = micros();   //Set the zero_timer for the first loop.
  loop_timer = micros();

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 1100;

}

void loop() { 
  Serial.println(receiver_input_channel_throttle);

  //Create ESC signal
  for (int idx = 0;idx<4;idx++) {
    timer_esc[idx] = receiver_input_channel_throttle;
  }

  DriveServos(timer_esc);  
}

void DriveServos(unsigned long timer_esc[4]) {
  //Make sure to wait 4ms
  while(micros() - loop_timer < 4000); //Apparently we need to wait 4000 us = 4ms
  loop_timer = micros();

  //Then we set pins 8,9,10,11 to high on PORTB
  PORTB |= B00001111;

  //Increment each esc timer by the loop timer. Assume the input is each esc us pulse.
  for (int idx = 0;idx<4;idx++) {
    timer_esc[idx] += loop_timer;
  }

  //Then we do a loop where we set each pin to low
  int ON = 4;
  while(ON > 0) {
    esc_loop_timer = micros();
    //Turn off pin 11
    if (timer_esc[0] <= esc_loop_timer) {
      PORTB &= 11110111;
      ON-=1;
    }
    //Turn off pin 10
    if (timer_esc[1] <= esc_loop_timer) {
      PORTB &= 11111011;
      ON-=1;
    }
    //Turn off pin 9
    if (timer_esc[2] <= esc_loop_timer) {
      PORTB &= 11111101;
      ON-=1;
    }
    //Turn off pin 8
    if (timer_esc[3] <= esc_loop_timer) {
      PORTB &= 11111110;
      ON-=1;
    }
  }
}

//Setup ESCs
void SetupESCs() {
  //Set pins 8,9,10,11 to outputs
  DDRB |= B00001111;
}

///INTERUPPTS FOR RECEIVER SIGNALS
ISR(PCINT2_vect) { //this is for pins 0-7
  current_time = micros();
  if (PIND & B00000100) {                                      //Is input 2 high?
    if(last_channel_throttle == 0){                                   //Input changed from 0 to 1
      last_channel_throttle = 1;                                      //Remember current input state
      timer_throttle = current_time;                                  //Set timer_throttle to current_time
    }
  }
  else if(last_channel_throttle == 1){                                //Input is not high and changed from 1 to 0
    last_channel_throttle = 0;                                        //Remember current input state
    receiver_input_channel_throttle = current_time - timer_throttle;         //Channel throttle is current_time - timer_throttle 
  }  
}
