/*
 * DoHIL.c
 *
 *  Created on: Aug 17, 2020
 *      Author: Sam Russ
 *      Edited to just send 1 number: Carlos Montalvo
 */
 
#include "HardwareSerial.h"
#include "Arduino.h"

//This is needed to convert from floats to longs
union inparser {
  long inversion;
  float floatversion;
} ;

// Declared these global so they can be "spied on" during debug
#define MAXLINE	60
char FirstLine[MAXLINE];

void SendReceiveNumber(){

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
  FirstLine[i] = '\0'; //Null terminate the string

	// Format from PC: (It's looking for a \r right?)
	// X:nnnnnnnn \r 

  //Echo back what you received.
  //Serial.print(FirstLine);
  //Serial.print('\r');

  //Convert Hex Digits to an integer
	inputvar.inversion = 0;
	for(i=2;i<10;i++){
		inputvar.inversion <<= 4;
		inputvar.inversion |= (FirstLine[i] <= '9' ? FirstLine[i] - '0' : toupper(FirstLine[i]) - 'A' + 10);
	}

  //Convert integer to float
	float number = inputvar.floatversion;

  //Multiply float by 2
  float outnumber = 2*number;

  //Convert out float to an integer
  inputvar.floatversion = outnumber;
  
  char outline[20];
  sprintf(outline, "H:%08lx \0",inputvar.inversion);
  Serial.print(outline);
  Serial.print('\r');

}
