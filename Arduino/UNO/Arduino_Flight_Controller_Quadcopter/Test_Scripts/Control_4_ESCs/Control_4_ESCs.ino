#include <Servo.h>
Servo esc[4];

byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;

void setup() {
  Serial.begin(9600);

  //Attach ESCs
  for (int idx = 0;idx<4;idx++) {
    //Using CleanFlight Notation
    esc[idx].attach(11-idx);
  }

  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 1100;

}

void loop() { 
  Serial.println(receiver_input_channel_throttle);
  for (int idx = 0;idx<4;idx++) {
    esc[idx].writeMicroseconds(receiver_input_channel_throttle);
  }
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
