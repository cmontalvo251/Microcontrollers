#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
File FlightTest;
long roll;
long pitch;
long deltaRoll;
long deltaPitch;
long rollc;
long pitchc;
long ratezc;
long rateyc;
long FinalRoll;
long FinalPitch;
int ch1;
int control1 = 7; 
const int analogInPin = 3; 
int sensorValue = 0;        
int outputValue = 0;  
unsigned long time1; 
unsigned long newtime = 0;
int button = 39;
int i = 1;
int k = 0;
int led1 = 37;
int led2 = 34 ;

#include <Servo.h>
Servo servo1;
Servo servo2;

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup(void)

{
  
  Serial.begin(9600);
  
  Serial.print("Looking for SD card...");
   pinMode(53, OUTPUT);
 
  if (!SD.begin(53)) {
    Serial.println("SD card not found.");
    digitalWrite(led2, LOW);
    return;  
  }
Serial.println("SD card found!");

digitalWrite(led2, HIGH);

  Serial.println("BNO055 Activated"); Serial.println("");
  

  if(!bno.begin())
  {
    digitalWrite(led1, LOW);
    Serial.print("BNO055 not detected.");
    while(1);
  }

  delay(1000);

  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");

  bno.setExtCrystalUse(true);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
  digitalWrite(led1, HIGH);
servo1.attach(5);
servo2.attach(10);

pinMode(ch1, INPUT);
pinMode(control1, OUTPUT);


}



void loop(void)
{

ch1 = pulseIn(8, HIGH);
  
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  

rollc = 0;
ratezc = 0;
pitchc = -3;
rateyc = 0;

servo1.write(FinalRoll);

pitch = euler.y();

deltaPitch = .6*(pitch-pitchc)-1.5*(gyro.y()-rateyc);

FinalPitch = (deltaPitch + 90);

servo2.write(FinalPitch);
  
roll = euler.z();

deltaRoll = .8*(roll-rollc)-2*(gyro.z()-ratezc);

FinalRoll = abs(deltaRoll + 90);

if (FinalRoll >=145)
{
  FinalRoll = 145;
}

if (FinalRoll <=35)

{
  FinalRoll = 35;
}

if (ch1 > 1500)

{
digitalWrite(control1, HIGH);
}

else
{
  digitalWrite(control1,LOW);  
}


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
 FlightTest = SD.open("9DOF.txt", FILE_WRITE);
 Serial.print("Open");
 Serial.println();
 Serial.print ("Data ");  Serial.print(i-(i/2));
 Serial.println();
 FlightTest.println();
 FlightTest.print ("Data "); FlightTest.print(i-(i/2));
 FlightTest.println();
 FlightTest.println("Millis  EulerP  EulerR  RateP   RateR ServoAngleR   ServoAngleP  Autopilot");
 


}
newtime = nowtime;

}


if (i%2 == 1)

{
  Serial.print("Standby");
  Serial.println();
  //digitalWrite(led1 ,LOW);
}

{

time1 = millis()-newtime; 
sensorValue = analogRead(analogInPin);                                   

FlightTest.print(time1); FlightTest.print("   ");
FlightTest.print(euler.y()); FlightTest.print("   ");
FlightTest.print(euler.z()); FlightTest.print("   ");
FlightTest.print(gyro.y()); FlightTest.print("   ");
FlightTest.print(gyro.z()); FlightTest.print("   ");
FlightTest.print(FinalRoll); FlightTest.print("   ");
FlightTest.print(FinalPitch); FlightTest.print("   ");
 if (ch1 > 1500)
 {
  FlightTest.println("1");
 }
 else
 {
  FlightTest.println("0");
 }

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

if (i%2 == 1 && i > 1)
{
  FlightTest.close();
}
     
}

