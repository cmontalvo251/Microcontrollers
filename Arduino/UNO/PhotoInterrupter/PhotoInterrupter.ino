void setup()
{  
  //attachInterrupt(<interruptPin>,<interruptServiceRoutine>,<trigger>);
  // interruptPin is 0 or 1, for digital pin 2 or 3, interruptServiceRoutine is the function to run
  // trigger is what will cause the ISR to run, can be LOW, RISING, FALLING or CHANGE
  attachInterrupt(0,computeAngularVelocity,FALLING);

  Serial.begin(9600);
}
double now_time = 0;
double dTtime = 0;
double w = 0;

//The main program loop that runs while the board is active
// This loop flashes the pin 13 led at 1Hz, each delay is 500ms

void loop()
{
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.print(w);
  Serial.println();
  delay(0.1);
}

void computeAngularVelocity()
{
  dTtime = millis()-now_time;
  now_time += dTtime;
  //use the time calculation and the number of spokes to calculate the rpm of the motor
  w = 90.0/dTtime;
}
