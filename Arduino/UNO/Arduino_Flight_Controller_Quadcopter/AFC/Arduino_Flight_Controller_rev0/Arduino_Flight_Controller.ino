//Declaring Variables
byte last_channel_throttle, last_channel_yaw, last_channel_pitch, last_channel_roll, last_channel_gear;
int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear;
unsigned long timer_channel_throttle, timer_channel_yaw, timer_channel_pitch, timer_channel_roll, timer_channel_gear;
unsigned long timer_throttle, timer_yaw, timer_pitch, timer_roll, timer_gear, current_time, zero_timer;
int esc_commands[4]; //0 = top left, 1 = bottom left, 2 = top right, 3 = bottom right
boolean QUADARMED;

//Setup routine
void setup(){

  ///Receiver Stuff
  PCICR |= (1 << PCIE2); 
  PCMSK2 |= (1 << PCINT18); //Pin 2 - Throttle
  PCMSK2 |= (1 << PCINT19); //Pin 3 - Yaw
  PCMSK2 |= (1 << PCINT20); //Pin 4 - Pitch
  PCMSK2 |= (1 << PCINT21); //Pin 5 - receiver_input_channel_roll
  PCMSK2 |= (1 << PCINT22); //Pin 6 - Gear
  zero_timer = micros();   //Set the zero_timer for the first loop.

  //Initialize Receiver Signals
  receiver_input_channel_throttle = 0;
  receiver_input_channel_yaw = 0;
  receiver_input_channel_roll = 0;
  receiver_input_channel_pitch = 0;
  receiver_input_channel_gear = 0;
  QUADARMED = 0;
  
  Serial.begin(9600);
}

//Main program loop
void loop(){

  //Make sure throttle is down before you arm
  if ((receiver_input_channel_throttle < 1200) and (receiver_input_channel_throttle > 1000) and (receiver_input_channel_gear > 1400) and (receiver_input_channel_gear < 1900)) {
    QUADARMED = true;
  }

  //If you switch off rotors make sure to disarm rotors
  if ((QUADARMED) and (receiver_input_channel_gear > 1800)) {
    QUADARMED = false;
  }

  //Based on the Gear Value you need to compute Acro mode or Attitude/Altitude Hold Mode
  //Gear switch = 1900,1500,1100
  if ((receiver_input_channel_gear < 1600) and (receiver_input_channel_gear > 1400) and QUADARMED) {
    //ACRO MODE
    Serial.print("ACRO MODE ");
    acro_mode(); 
  } else {
    //If Gear is giving something wonky just default to 1100
    for (int idx = 0;idx<4;idx++) {
      esc_commands[idx] = 1100;
    }
  }

  //Make sure to add the saturation blocks
  for (int idx = 0;idx<4;idx++) {
    if (esc_commands[idx] > 1900) {
      esc_commands[idx] = 1900;
    }
    if (esc_commands[idx] < 1100) {
      esc_commands[idx] = 1100;
    }
  }
    

  //Print Controller Health
  Serial.print("T = ");
  Serial.print(receiver_input_channel_throttle);
  Serial.print(" ");

  Serial.print("Y = ");
  Serial.print(receiver_input_channel_yaw);
  Serial.print(" ");

  Serial.print("P = ");
  Serial.print(receiver_input_channel_pitch);
  Serial.print(" ");

  Serial.print("R = ");
  Serial.print(receiver_input_channel_roll);
  Serial.print(" ");

  Serial.print("G = ");
  Serial.print(receiver_input_channel_gear);
  Serial.print(" ");

  Serial.print("TL = ");
  Serial.print(esc_commands[0]);
  Serial.print(" ");

  Serial.print("BL = ");
  Serial.print(esc_commands[1]);
  Serial.print(" ");

  Serial.print("TR = ");
  Serial.print(esc_commands[2]);
  Serial.print(" ");

  Serial.print("BR = ");
  Serial.print(esc_commands[3]);
  Serial.print(" ");

  Serial.print("\n");
}

//Compute Fancy Throttle
int throttle_curve(int tin) {
  float dt = float(tin)-1100;
  //float tout = p1 + p2*dt + p3*dt^2 + p4*dt^3 + p5*dt^4 + p6*dt^5;
  //for now just do linear
  float tout = 1100 + 1.0*dt;
  return int(tout);
}

//Acro Mode Computation
void acro_mode() {
  //Compute Throttle based on receiver_input_channel
  int throttle_out = throttle_curve(receiver_input_channel_throttle);

  //Set rate of receiver_input_channel_roll/pitch and yaw
  float rate = 1.0; //DO NOT SET RATE > 1!!!!!
  float rate_yaw = rate/2.0;

  //Generate Commands to ESCs
  //Top Left
  esc_commands[0] = throttle_out + rate*(receiver_input_channel_roll-1500) - rate*(receiver_input_channel_pitch-1500) + rate_yaw*(receiver_input_channel_yaw-1500);
  //Bottom Left
  esc_commands[1] = throttle_out + rate*(receiver_input_channel_roll-1500) + rate*(receiver_input_channel_pitch-1500) - rate_yaw*(receiver_input_channel_yaw-1500);
  //Top Right
  esc_commands[2] = throttle_out - rate*(receiver_input_channel_roll-1500) - rate*(receiver_input_channel_pitch-1500) - rate_yaw*(receiver_input_channel_yaw-1500);
  //Bottom Right
  esc_commands[3] = throttle_out - rate*(receiver_input_channel_roll-1500) + rate*(receiver_input_channel_pitch-1500) + rate_yaw*(receiver_input_channel_yaw-1500);
}

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

