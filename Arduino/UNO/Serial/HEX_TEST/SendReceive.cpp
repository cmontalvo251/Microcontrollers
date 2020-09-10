/*
   DoHIL.c

    Created on: Aug 17, 2020
        Author: Sam Russ
        Edited to just send 1 number: Carlos Montalvo
*/

#include "HardwareSerial.h"
#include "Arduino.h"

//This is needed to convert from floats to longs
union inparser {
  unsigned long inversion;
  float floatversion;
} ;

// Declared these global so they can be "spied on" during debug
#define MAXLINE	60
char FirstLine[MAXLINE];

void SendReceiveNumber() {

  char inchar;
  int i = 0;

  union inparser inputvar;

  FirstLine[0] = 'H';
  FirstLine[1] = ':';
  FirstLine[2] = '4';
  FirstLine[3] = '0';
  FirstLine[4] = '9';
  FirstLine[5] = '9';
  FirstLine[6] = '9';
  FirstLine[7] = '9';
  FirstLine[8] = '9';
  FirstLine[9] = 'a';
  FirstLine[10] = '\0'; //Null terminate the string

  //Echo back what you received.
  Serial.print("HEX In: ");
  Serial.print(FirstLine);
  Serial.print('\n');

  //Convert Hex Digits to an integer
  inputvar.inversion = 0;
  for (i = 2; i < 10; i++) {
    inputvar.inversion <<= 4;
    inputvar.inversion |= (FirstLine[i] <= '9' ? FirstLine[i] - '0' : toupper(FirstLine[i]) - 'A' + 10);
  }

  Serial.print("Integer Received = ");
  Serial.print(inputvar.inversion);

  //Convert integer to float
  float number = inputvar.floatversion;

  Serial.print(" Float Received = ");
  Serial.print(number);
  Serial.print("\n");

  //Multiply float by 2
  //float outnumber = 2*number;

  //Convert out float to an integer
  //inputvar.floatversion = outnumber;
  //int int_outvar = inputvar.inversion;

  Serial.print("Integer Sent out = ");
  Serial.print(inputvar.inversion);

  char outline[20];
  sprintf(outline, "H:%08lx ", inputvar.inversion);
  Serial.print(" HEX Out = ");
  Serial.print(outline);
  Serial.print('\n');

}
