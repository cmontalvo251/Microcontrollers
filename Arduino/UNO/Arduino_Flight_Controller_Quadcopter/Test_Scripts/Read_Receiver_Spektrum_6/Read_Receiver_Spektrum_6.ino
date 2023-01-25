//Declaring Variables
byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;

//Setup routine
void setup(){
  //B means binary and DDRD is a certain hex address on the arduino. |= means set this hex address to this binary
  //state of ones. Wherever there is a one the state is changed wherever there is a 0 the state is unchanged.
  //DDRD =  B00000001; //Set to that exact state
  //DDRD |= B00000001; //Only change that last bit to a 1
  //DDRD &= B11111110; //Only change that last bit to a 0
  //This line isn't needed because I just use the Servo.h library.
  //DDRD |=   B11110000;                                 //Configure digital port 4, 5, 6 and 7 as output
  //1111 means make 4,5,6,7 to output while 3,2,1,0 are left alone
  //DDRB |= B00010000;                                 //Configure digital poort 12 as output
  //Port 12 is for the LED so you don't need this
  //So this leaves 8-15 alone and sets 12 to high or output mode
  
  //PORTD is pins 0-7 while PORTB is pins 8-15. 
  
  //Arduino Uno pins default to inputs, so they don't need to be explicitly declared as inputs
  
  //Allows the arduino to run in interrupt mode so if pins - 8-11 change an interrupt secquence ISR will run
  //PCICR |= (1 << PCIE0);                             // set PCIE0 to enable PCMSK0 scan
  //PCMSK0 |= (1 << PCINT0);                           // set PCINT0 (digital input 8) to trigger an interrupt on state change
  //PCMSK0 |= (1 << PCINT1);                           // set PCINT1 (digital input 9)to trigger an interrupt on state change
  //PCMSK0 |= (1 << PCINT2);                           // set PCINT2 (digital input 10)to trigger an interrupt on state change
  //PCMSK0 |= (1 << PCINT3);                           // set PCINT3 (digital input 11)to trigger an interrupt on state change

  //PCIE2 is pins 0-7. Although the spektrum is a 6-channel receiver
  //we really only need 5 channels to get the thing going right.
  //So we just need pins 3,4,5,6,7
  PCICR |= (1 << PCIE2); 
  //PCMSK2 |= (1 << PCINT17); //Pin 1 - DOESN'T WORK. Probably because of TX/RX
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  PCMSK2 |= (1 << PCINT19); //Pin 3 - Yaw
  PCMSK2 |= (1 << PCINT20); //Pin 4 - Pitch
  PCMSK2 |= (1 << PCINT21); //Pin 5 - Roll
  PCMSK2 |= (1 << PCINT22); //Pin 6 - Gear
  //PCMSK2 |= (1 << PCINT23); //Pin 7 - ??
  zero_timer = micros();   //Set the zero_timer for the first loop.
  
  receiver_input_channel_throttle = 0;
  receiver_input_channel_yaw = 0;
  receiver_input_channel_roll = 0;
  receiver_input_channel_pitch = 0;
  receiver_input_channel_gear = 0;
  
  Serial.begin(9600);
}

//Main program loop
void loop(){
  delay(10);

  Serial.print("Throttle = ");
  Serial.print(receiver_input_channel_throttle);
  Serial.print(" ");

  Serial.print("Yaw = ");
  Serial.print(receiver_input_channel_yaw);
  Serial.print(" ");

  Serial.print("Pitch = ");
  Serial.print(receiver_input_channel_pitch);
  Serial.print(" ");

  Serial.print("Roll = ");
  Serial.print(receiver_input_channel_roll);
  Serial.print(" ");

  Serial.print("Gear = ");
  Serial.print(receiver_input_channel_gear);
  Serial.print(" ");

  Serial.print("\n");
}

//This routine is called every time input 8, 9, 10 or 11 changed state - remember that 8-11 is the receiver input channel
//this routine grabs when the receiver goes high and when it goes low. The different between the two times is the pulse length
//which gives the PWM signal length to output to PORTD. PINB is the current state of PORTB which is pins 8-15. 
//ISR(PCINT0_vect){ //This is for pins 8-15
ISR(PCINT2_vect) { //this is for pins 0-7
  current_time = micros();
  //Channel throttle=========================================
  //if(PINB & B00000001){                                        //Is input 8 high?
  //if (PIND & B00001000) {                                      //Is input 3 high?
  if (PIND & B00000100) {                                      //Is input 2 high?
    if(last_channel_throttle == 0){                                   //Input changed from 0 to 1
      last_channel_throttle = 1;                                      //Remember current input state
      timer_throttle = current_time;                                  //Set timer_throttle to current_time
    }
  }
  else if(last_channel_throttle == 1){                                //Input is not high and changed from 1 to 0
    last_channel_throttle = 0;                                        //Remember current input state
    receiver_input_channel_throttle = current_time - timer_throttle;         //Channel throttle is current_time - timer_throttle 
    //which is essentially the length of the pulse
  }

  if (PIND & B00001000) {
    if (last_channel_yaw == 0) {
      last_channel_yaw = 1;
      timer_yaw = current_time;
    }
  }
  else if (last_channel_yaw == 1) {
    last_channel_yaw = 0;
    receiver_input_channel_yaw = current_time - timer_yaw;
  }

  if (PIND & B00010000) {
    if (last_channel_pitch == 0) {
      last_channel_pitch = 1;
      timer_pitch = current_time;
    }
  }
  else if (last_channel_pitch == 1) {
    last_channel_pitch = 0;
    receiver_input_channel_pitch = current_time - timer_pitch;
  }

  if (PIND & B00100000) {
    if (last_channel_roll == 0) {
      last_channel_roll = 1;
      timer_roll = current_time;
    }
  }
  else if (last_channel_roll == 1) {
    last_channel_roll = 0;
    receiver_input_channel_roll = current_time - timer_roll;
  }

  if (PIND & B01000000) {
    if (last_channel_gear == 0) {
      last_channel_gear = 1;
      timer_gear = current_time;
    }
  }
  else if (last_channel_gear == 1) {
    last_channel_gear = 0;
    receiver_input_channel_gear = current_time - timer_gear;
  }
  
}

