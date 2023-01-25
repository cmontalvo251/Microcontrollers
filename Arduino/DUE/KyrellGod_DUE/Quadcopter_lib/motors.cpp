/*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <math.h>
#include <Arduino.h>
#include "motors.h"

motors::motors()
{
	// all initializations go into the setup function
}

motors::~motors()
{
}

void motors::motorsSetup()
{
	motorsReleased = false;
	motorsPanicStopped = false;
	
	///This max arduinoMaxPulseLength is 1,000,000/MOTORS_ESC_PWM_FREQ
	///MOTORS_ESC_PWM_FREQ = 400
	///so arduinoMaxPulseLength = 2500 - not sure what the significance of this number is
	//I think this just means that the arduino can't send a signal past 2500
	arduinoMaxPulseLenght = 1000000.0f/MOTORS_ESC_PWM_FREQ;

	//ok so input is MOTORS_ESC_PWM_MIN = 1000.
	//x-axis = 0 to 2500
	//MOTORS_12BIT_LIMIT = PWM_RESOLUTION_MAX = 4095
	//y-axis = 0 to 4095
	float tmp = mapp<float>(MOTORS_ESC_PWM_MIN, 0.0f, arduinoMaxPulseLenght, 0.0f, (float) MOTORS_12BIT_LIMIT);
	//Not sure why this is here. The ceil function gives you the nearest integer but doesn't
	//the integer routine do the same? Unless they want to make sure that the nearest integer
	//is the one above and not below
	tmp = ceil(tmp);
	//anyway so lowerClippingValue = 1000*(4095/2500)
	lowerClippingValue = (int) tmp;	///this just sets some upper and lower values 
	
	// define pins as output
	pinMode(MOTORS_PIN_PWM0, OUTPUT); ///this sets ESCs as outputs
	pinMode(MOTORS_PIN_PWM1, OUTPUT); ///Motors are connected to pins 6,7,8,9
	pinMode(MOTORS_PIN_PWM2, OUTPUT);
	pinMode(MOTORS_PIN_PWM3, OUTPUT);

	///this routine goes back to flightcontroller.cpp
}

void motors::sendFullSignal()
{	
	analogWrite(MOTORS_PIN_PWM0, MOTORS_12BIT_LIMIT);
	analogWrite(MOTORS_PIN_PWM1, MOTORS_12BIT_LIMIT);
	analogWrite(MOTORS_PIN_PWM2, MOTORS_12BIT_LIMIT);
	analogWrite(MOTORS_PIN_PWM3, MOTORS_12BIT_LIMIT);
	
	#ifdef DBG_MOTORS_PWM_PRINT
	printPWMData(MOTORS_12BIT_LIMIT, MOTORS_12BIT_LIMIT, MOTORS_12BIT_LIMIT, MOTORS_12BIT_LIMIT);
	#endif	
}

void motors::sendZeroSignal()
{	
	analogWrite(MOTORS_PIN_PWM0, 0);
	analogWrite(MOTORS_PIN_PWM1, 0);
	analogWrite(MOTORS_PIN_PWM2, 0);
	analogWrite(MOTORS_PIN_PWM3, 0);
	
	#ifdef DBG_MOTORS_PWM_PRINT
	printPWMData(0.0f, 0.0f, 0.0f, 0.0f);
	#endif	
}

void motors::sendMinSignal()
{	
	///MOTORS_ESC_PWM_MIN = 1000.
	convert2MotorSignal(MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MIN);
}

void motors::release()
{	
	motorsReleased = true;
}

void motors::block()
{	
	motorsReleased = false;
}

void motors::panicStop()
{
	sendMinSignal();
	motorsPanicStopped = true;
}
	
void motors::feedStabilizerPWM(const Vector4f pwmDutyCycle)
{
	if(motorsReleased == false || motorsPanicStopped == true)
	{
		sendMinSignal();
		return;
	}

	//Send the 4 signals from pwm_output
	convert2MotorSignal(pwmDutyCycle.x, pwmDutyCycle.y , pwmDutyCycle.z, pwmDutyCycle.q);
}

///This routine takes 4 PWM Signals and sends them to the pins using a 12bit number
///Since this uses 12 bit numbers
void motors::convert2MotorSignal(float pwm0, float pwm1, float pwm2, float pwm3)
{
	// clip input pulse widths
	///Makes sure you don't send a value below 1000 or over 2000
        //Constrain PWM signal from 1000 to 2000
	float motor0 = constrainn<float>(pwm0, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MAX);
	float motor1 = constrainn<float>(pwm1, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MAX);
	float motor2 = constrainn<float>(pwm2, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MAX);
	float motor3 = constrainn<float>(pwm3, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MAX);
	
	///Debug print to Serial
	#ifdef DBG_MOTORS_PWM_PRINT
	printPWMData(pwm0, pwm1, pwm2, pwm3);
	#endif		
	
	// map to maximum range with 12 Bit resolution
	///Ok from above the arduinoMaxPulseLength = 2500
	///MOTORS_12BIT_LIMIT = PWM_RESOLUTION_MAX
	///This is an interpolation routine - motorout = m*x + b
	///b in this case is 0.0f and the slope is 
	///m = (MOTORS_12BIT_LIMIT)/arduinoMaxPulseLength
	///m = PWM_RESOLUTION_MAX/arduinoMaxPulseLength
	///Turns out PWM_RESOLUTION_MAX is set in global.h
	///This is set to 4095 which is 2^12-1
	///or 12 bits. An 8-bit register would be 255.
	///m = 4095/2500
	///So then the motor output is just 
	///motor_out = (4095/2500)*pwm_signal

	//Xaxis is 0 to 2500
	//Yaxis is 0 to 4095
	//so motor0 = motor0*(4095/2500)
	motor0 = mapp<float>(motor0, 0.0f, arduinoMaxPulseLenght, 0.0f, (float) MOTORS_12BIT_LIMIT);
	motor1 = mapp<float>(motor1, 0.0f, arduinoMaxPulseLenght, 0.0f, (float) MOTORS_12BIT_LIMIT);
	motor2 = mapp<float>(motor2, 0.0f, arduinoMaxPulseLenght, 0.0f, (float) MOTORS_12BIT_LIMIT);
	motor3 = mapp<float>(motor3, 0.0f, arduinoMaxPulseLenght, 0.0f, (float) MOTORS_12BIT_LIMIT);
	
	// convert float to integer values
	int motor0i = (int) motor0;
	int motor1i = (int) motor1;
	int motor2i = (int) motor2;
	int motor3i = (int) motor3;
	
	// clip integers
	///this constrains motor0i between the lowerClippingValue and PWM_RESOLUTION_MAX
	///The lowerClippingValue is equal to m*1000
	//I wrote that comment above before ok.... I see m = 4095/2500
	//lowerClippingValue = 1000*(4095/2500)
	//so this constraint value says keep motor signals between
	//1000*(4095/2500) and 2500*(4095/2500) or 2500
	motor0i = constrainn<int>(motor0i, lowerClippingValue, MOTORS_12BIT_LIMIT);
	motor1i = constrainn<int>(motor1i, lowerClippingValue, MOTORS_12BIT_LIMIT);
	motor2i = constrainn<int>(motor2i, lowerClippingValue, MOTORS_12BIT_LIMIT);
	motor3i = constrainn<int>(motor3i, lowerClippingValue, MOTORS_12BIT_LIMIT);
	
	// set new motor output
	///Ok then he just sends these signals to each motor.
	///Pretty wierd that he goes from PWM duty cycle to this wierd value.
	///I think I need to test this function on the DUE or UNO to see what it does
	//Then we analog write this 12 Bit number
	analogWrite(MOTORS_PIN_PWM0, motor0i);
	analogWrite(MOTORS_PIN_PWM1, motor1i);
	analogWrite(MOTORS_PIN_PWM2, motor2i);
	analogWrite(MOTORS_PIN_PWM3, motor3i);	
}

#ifdef DBG_MOTORS_PWM_PRINT
void motors::printPWMData(float m0, float m1, float m2, float m3)
{
	Serial.print("\nM0 = "); Serial.print(m0); 
	Serial.print(" M1 = "); Serial.print(m1); 
	Serial.print(" M2 = "); Serial.print(m2);
	Serial.print(" M3 = "); Serial.println(m3);
}
#endif	
