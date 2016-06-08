#include <SD.h>

File Airspeed;

const int analogInPin = 3; 
int sensorValue = 0;        
int outputValue = 0;  
unsigned long time1; 
unsigned long newtime = 0;
int button = 2;
int i = 1;
int k = 0;
void setup() {

{
  Serial.begin(9600);
  
  Serial.print("Looking for SD card...");
   pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("SD card not found.");
    return;  
  }
Serial.println("SD card found!");
}
}


void loop() {

unsigned long nowtime = millis();

if (digitalRead(button) == LOW && k > 0)
{
  --k;
}

if (digitalRead(button) == HIGH && k < 1)

{ ++i;
  ++k;
if (i%2 == 0)
{
 Airspeed = SD.open("Airspeed.txt", FILE_WRITE);
 Serial.print("Open");
 Serial.println();
 Serial.print ("Data ");  Serial.print(i-(i/2));
 Serial.println();
 Airspeed.println();
 Airspeed.print ("Data "); Airspeed.print(i-(i/2));
 Airspeed.println();
}
newtime = nowtime;
}


if (i%2 == 1)

{
  Serial.print("Standby");
  Serial.println();
}

else

{
time1 = millis()-newtime; 
sensorValue = analogRead(analogInPin);                                   

  Serial.print(time1);
  Serial.print (" ");      
  Serial.print(sensorValue);
  Serial.println();
  Airspeed.print(time1);
  Airspeed.print (" ");
  Airspeed.print(sensorValue);
  Airspeed.println();
}


if (i%2 == 1 && i > 1)
{
  Airspeed.close();
  //can change to Airspeed.flush()
  //make sure to do that in the next iteration
}
  
 delay(2);   
}
