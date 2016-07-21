#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 6; //Make sure this is the same as processing code
int numPitot = 3;
float data[6]; //You must hardcode the number of variables.
float cal_voltage[3];
float sensorvalue;
float airspeed_then[3];
int analogInPin[3]; //Airspeed sensor is hooked up to pin 3
float airspeed[3];
float airspeed_now[3];

void setup() 
{
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
  //Serial.print("Cal Voltage = ");
  //Serial.print(cal_voltage);
  //Serial.print("\n");
  digitalWrite(13,LOW);

  for (int idx = 0;idx<numPitot;idx++) {
    analogInPin[idx] = idx+1;
  }
  digitalWrite(13,LOW);
  //Read pitot sensor N times to calibrate sensor
  for (int idx = 0;idx<numPitot;idx++) {
      cal_voltage[idx] = 0;
  }
  int N = 10;
  for (int idx = 0;idx<N;idx++) {
    for (int jdx = 0;jdx<numPitot;jdx++){
      sensorvalue = analogRead(analogInPin[jdx]);
      cal_voltage[jdx]+=sensorvalue*(5.0/1023.0);
      }
      //delay(10);
  }
  for (int idx = 0;idx<numPitot;idx++) {
    cal_voltage[idx] = cal_voltage[idx]/float(N);
  }
}


void loop()
{
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)

  //Pull Data from Airspeed sensor
  for (int idx = 0;idx<numPitot;idx++) {
    float sensorvalue = analogRead(analogInPin[idx]);
    //Convert to voltage
    float raw_voltage = sensorvalue*(5.0/1023.0);
    //Calibrate voltage
    float voltage = raw_voltage - cal_voltage[idx];
    //Convert to pressure
    float pressure = voltage/101.325;
    //Compute Airspeed using bernoulli
    float k = pow(pressure+1.0,2.0/7.0);
    //Check for error
    if (k < 1.0) {
      k = 1.0;
    }
    //Compute speed of sound
    //Based on temperature in Celsius
    float tempC = 20.0;
    float tempK = 273.15 + tempC;
    float a_inf = sqrt(1.4*286.0*tempK);
    //Compute Airspeed
    airspeed_now[idx] = a_inf*(sqrt(5.0*(k-1.0)));
    //Use a complimentary filter to filter out noise
    float sigma = 0.03;
    airspeed[idx] = (1.0-sigma)*airspeed_then[idx] + sigma*airspeed_now[idx];
    airspeed_then[idx] = airspeed[idx];
    data[idx] = airspeed[idx];
//    Serial.print("Raw Bits = ");
//    Serial.print(sensorvalue);
//    Serial.print(" Voltage = ");
//    Serial.print(voltage);
//    Serial.print(" Airspeed = ");
//    Serial.print(airspeed[idx]);
//    Serial.print("\n");
  }
  //Random Number
  for (int idx = numPitot;idx<6;idx++){
     data[idx] = random(1,10);
  }
  

  
  //Write Data to Serial
  serialPrintFloatArr(data,numVars);
  Serial.println("\r\n"); //line break. Tells processing to stop reading data

  //wait a bit so you don't write super freaking fast
  //delay(10);
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}

