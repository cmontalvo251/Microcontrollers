/*
 * DoHIL.c
 *
 *  Created on: Aug 17, 2020
 *      Author: Sam Russ
 *      Edited to just send 1 number: Carlos Montalvo
 */
 
#include "HardwareSerial.h"
#include "Arduino.h"

#include "I2Csensors.h"
#include "ADCS_Algo.h"


// Declared these global so they can be "spied on" during debug
#define MAXLINE	60
char FirstLine[MAXLINE];

void DoHardwareInLoop(){

	char inchar;
	int i = 0;

	union inparser inputvar;

	do {
    do{
      inchar = Serial.read();
    } while (inchar == -1);

		FirstLine[i++] = inchar;
		//PRINTF("%c",inchar);
	} while ((inchar != '\r') && (i<MAXLINE));
  FirstLine[i] = '\0'; //Null terminate the character

	// Format from PC: (It's looking for a \r right?)
	// X:nnnnnnnn \r 

  //Echo back what you received.
  Serial.print(FirstLine);
  Serial.print('\r');

	inputvar.inversion = 0;

	for(i=2;i<10;i++){
		inputvar.inversion <<= 4;
		inputvar.inversion |= (FirstLine[i] <= '9' ? FirstLine[i] - '0' : toupper(FirstLine[i]) - 'A' + 10);
	}

	//double number = inputvar.floatversion;
  //double outnumber = 2*number;
  
  //char output[8];
  //sprintf(output, "%08x",outnumber);
  //Serial.print("H:0x");
  //Serial.print(output);
  //Serial.print('\r');


}
