//Declaring Variables
byte last_channel_1, last_channel_2, last_channel_3, last_channel_4;
int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int counter_channel_1, counter_channel_2, counter_channel_3, counter_channel_4, start;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long zero_timer, timer_1, timer_2, timer_3, timer_4, current_time;

//The ESCs are plugged into PORTD - pins 4-7
//The receiver is plugged into PORTB - pins 8-11
//An LED is plugged into pin 12

//Setup routine
void setup(){
  //B means binary and DDRD is a certain hex address on the arduino. |= means set this hex address to this binary
  //state of ones. Wherever there is a one the state is changed wherever there is a 0 the state is unchanged.
  //DDRD =  B00000001; //Set to that exact state
  //DDRD |= B00000001; //Only change that last bit to a 1
  //DDRD &= B11111110; //Only change that last bit to a 0
  DDRD |=   B11110000;                                 //Configure digital poort 4, 5, 6 and 7 as output
  //1111 means make 4,5,6,7 to output while 3,2,1,0 are left alone
  DDRB |= B00010000;                                 //Configure digital poort 12 as output
  //Port 12 is for the LED so you may not need this
  //So this leaves 8-15 alone and sets 12 to high or output mode
  //PORTD is pins 0-7 while PORTB is pins 8-15. 
  //Arduino Uno pins default to inputs, so they don't need to be explicitly declared as inputs
  
  //Allows the arduino to run in interrupt mode so if pins - 8-11 change an interrupt secquence ISR will run
  PCICR |= (1 << PCIE0);                             // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT0);                           // set PCINT0 (digital input 8) to trigger an interrupt on state change
  PCMSK0 |= (1 << PCINT1);                           // set PCINT1 (digital input 9)to trigger an interrupt on state change
  PCMSK0 |= (1 << PCINT2);                           // set PCINT2 (digital input 10)to trigger an interrupt on state change
  PCMSK0 |= (1 << PCINT3);                           // set PCINT3 (digital input 11)to trigger an interrupt on state change
  
  ///Wait until the receiver is active and the throtle is set to the lower position.
  //receiver channel 3 is the left stick (LEFT/RIGHT) and receiver channel 4 is the left stick (UP/DOWN)
  while(receiver_input_channel_3 < 990 || receiver_input_channel_3 > 1020 || receiver_input_channel_4 < 1400){
    start ++;                                        //While waiting increment start whith every loop.
    //We don't want the esc's to be beeping annoyingly. So let's give them a 1000us puls while waiting for the receiver inputs.
    //The ESCs respond on a map of 1000us for no throttle and 2000us for full throttle. So sending 1000us will not do anything.
    PORTD |= B11110000;                              //Set digital poort 4, 5, 6 and 7 high.
    delayMicroseconds(1000);                         //Wait 1000us 
    PORTD &= B00001111;                              //Set digital poort 4, 5, 6 and 7 low.
    ///Remember that portD is 0-7 so the first command sets 4-7 to high and then the second command sets 4-7 to low
    delay(3);                                        //Wait 3 milliseconds before the next loop.
    if(start == 125){                                //Every 125 loops (500ms).
      digitalWrite(12, !digitalRead(12));            //Change the led status.
      start = 0;                                     //Start again at 0.
    }
  }
  //Once the left stick is centered and down the while loop breaks out
  start = 0;
  digitalWrite(12, LOW);                             //Turn off the led.
  zero_timer = micros();                             //Set the zero_timer for the first loop.
}

//Main program loop
void loop(){
  while(zero_timer + 4000 > micros());                       //Start the pulse after 4000 micro seconds.
  //^this is a fancy delay function without using the delay() function. It makes sure the loop here only runs every 4000 micro
  //seconds
  zero_timer = micros();                                     //Reset the zero timer.
  //This sets the pins 4-7 to high which begins the pulse
  PORTD |= B11110000;
  timer_channel_1 = receiver_input_channel_3 + zero_timer;   //Calculate the time when digital port 4 is set low.
  timer_channel_2 = receiver_input_channel_3 + zero_timer;   //Calculate the time when digital port 5 is set low.
  timer_channel_3 = receiver_input_channel_3 + zero_timer;   //Calculate the time when digital port 6 is set low.
  timer_channel_4 = receiver_input_channel_3 + zero_timer;   //Calculate the time when digital port 7 is set low.
  //This computes when to stop the pulse. In this case they are all the same.
  
  //16 in binary is 0001000 and 15 is 00001111. Thus if PORTD is 16 or higher it means that one of the escs is on.
  //So if you're doing one ESC it still needs to be 16
  while(PORTD >= 16){                                        //Execute the loop until digital port 4-7 is low.
    esc_loop_timer = micros();                               //Check the current time.
    //Remember that &= means only change the 0 bit to a zero but leave everything else unchanged. 
    // &= is the same as += so PORTD &= is the same as PORTD = PORTD & 
    if(timer_channel_1 <= esc_loop_timer)PORTD &= B11101111; //When the delay time is expired, digital port 4 is set low.
    if(timer_channel_2 <= esc_loop_timer)PORTD &= B11011111; //When the delay time is expired, digital port 5 is set low.
    if(timer_channel_3 <= esc_loop_timer)PORTD &= B10111111; //When the delay time is expired, digital port 6 is set low.
    if(timer_channel_4 <= esc_loop_timer)PORTD &= B01111111; //When the delay time is expired, digital port 7 is set low.
  }
}

//This routine is called every time input 8, 9, 10 or 11 changed state - remember that 8-11 is the receiver input channel
//this routine grabs when the receiver goes high and when it goes low. The different between the two times is the pulse length
//which gives the PWM signal length to output to PORTD. PINB is the current state of PORTB which is pins 8-15. 
ISR(PCINT0_vect){
  current_time = micros();
  //Channel 1=========================================
  if(PINB & B00000001){                                        //Is input 8 high?
    if(last_channel_1 == 0){                                   //Input 8 changed from 0 to 1
      last_channel_1 = 1;                                      //Remember current input state
      timer_1 = current_time;                                  //Set timer_1 to current_time
    }
  }
  else if(last_channel_1 == 1){                                //Input 8 is not high and changed from 1 to 0
    last_channel_1 = 0;                                        //Remember current input state
    receiver_input_channel_1 = current_time - timer_1;         //Channel 1 is current_time - timer_1 
    //which is essentially the length of the pulse
  }
  //Channel 2=========================================
  if(PINB & B00000010 ){                                       //Is input 9 high?
    if(last_channel_2 == 0){                                   //Input 9 changed from 0 to 1
      last_channel_2 = 1;                                      //Remember current input state
      timer_2 = current_time;                                  //Set timer_2 to current_time
    }
  }
  else if(last_channel_2 == 1){                                //Input 9 is not high and changed from 1 to 0
    last_channel_2 = 0;                                        //Remember current input state
    receiver_input_channel_2 = current_time - timer_2;         //Channel 2 is current_time - timer_2
  }
  //Channel 3=========================================
  if(PINB & B00000100 ){                                       //Is input 10 high?
    if(last_channel_3 == 0){                                   //Input 10 changed from 0 to 1
      last_channel_3 = 1;                                      //Remember current input state
      timer_3 = current_time;                                  //Set timer_3 to current_time
    }
  }
  else if(last_channel_3 == 1){                                //Input 10 is not high and changed from 1 to 0
    last_channel_3 = 0;                                        //Remember current input state
    receiver_input_channel_3 = current_time - timer_3;         //Channel 3 is current_time - timer_3
  }
  //Channel 4=========================================
  if(PINB & B00001000 ){                                       //Is input 11 high?
    if(last_channel_4 == 0){                                   //Input 11 changed from 0 to 1
      last_channel_4 = 1;                                      //Remember current input state
      timer_4 = current_time;                                  //Set timer_4 to current_time
    }
  }
  else if(last_channel_4 == 1){                                //Input 11 is not high and changed from 1 to 0
    last_channel_4 = 0;                                        //Remember current input state
    receiver_input_channel_4 = current_time - timer_4;         //Channel 4 is current_time - timer_4
  }
}

