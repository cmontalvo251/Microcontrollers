//Sender Code
 int fsrAnalogPin=A0;
int fsrReading; 
#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7,8);
const byte address[6]="00001";


void setup() {
 Serial.begin(9600);
 //pinMode(A0, INPUT);
 radio.begin();
 radio.openWritingPipe(address);
 radio.setPALevel(RF24_PA_MIN);
 radio.stopListening();
}

void loop() 
{
 // delay(100);
 fsrReading=analogRead(fsrAnalogPiconst char fsrReading[]="HELLO WORLD";
 

  radio.write(&fsrReading,sizeof(fsrReading));

  Serial.println(fsrReading);

}
