#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

int IN1=7;
int IN2=8;
int ENA=9;
float sum;
float accelbuff;
float I=0.0014541521;
void setup(void) 



{
  Serial.begin(9600);
  Serial.println("gyro and accel data"); Serial.println("");
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
    
   
  }
   delay (1000);
  bno.setExtCrystalUse(true);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);  
}



void loop(void) 
{
 imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
 imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
Serial.print('\n');
Serial.print("Gyro z ");
Serial.print(gyro.z());
int V=abs((2*(gyro.z())*I)/0.0022);
Serial.print (" V ");
Serial.print (V);
int motorVal = V*255/12;
Serial.print("Motor Val = ");
Serial.print(motorVal);
 
analogWrite(ENA,V*255/12);

 if ((gyro.z())> 0.001){
 digitalWrite(IN1,LOW);// rotate forward
 digitalWrite(IN2,HIGH);
 Serial.write(" forward");
 }
 else if ((gyro.z()) < -0.001){
 digitalWrite(IN1,HIGH);// rotate reverse
 digitalWrite(IN2,LOW);
 Serial.write(" backwards");
 }

 else if (abs(gyro.z())<=0.001){
  analogWrite(ENA, 0);
  Serial.write(" still");
 }
 delay(1000);
 analogWrite(ENA,0);
 delay(1000);


}


