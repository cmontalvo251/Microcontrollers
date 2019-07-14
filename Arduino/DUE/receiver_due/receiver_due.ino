//const int pwmPin = 13;
const int inPin = 11;
const int pulseLenIn = 100; //This was 50. Changing to 100 had no effect. 
int pulseLen;
volatile boolean l;
#define FREQUENCY 1000000 //What does this number do? Original value - 1000000

#include <Servo.h>

//When using Servo.h you can't recreate this code TC3 because
//it is used by Servo.cpp
//Probably can look into that later.
//void TC3_Handler()
//{
//  TC_GetStatus(TC1, 0);
  //digitalWrite(pwmPin, l = !l);
//}

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

void setup(void)
{
  //pinMode(pwmPin,OUTPUT);
  startTimer(TC1, 0, TC4_IRQn, FREQUENCY/pulseLenIn); //TC1 channel 0, the IRQ for that channel and the desired frequency
  Serial.begin(115200);
  pinMode(inPin,INPUT);
}

void loop(void)
{
  pulseLen = pulseIn(inPin, HIGH, FREQUENCY);
  
  /* should print pulseLenIn */
  Serial.print("PulseIn: ");
  Serial.print(pulseLen);
  Serial.println();
}
