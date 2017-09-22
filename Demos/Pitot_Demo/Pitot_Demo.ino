#include <Wire.h>
#include "CommunicationUtils.h"

int numVars = 6; //Make sure this is the same as processing code
int numPitot = 1;
float data[6]; //You must hardcode the number of variables.
float cal_voltage[4];
float sensorvalue;
float airspeed_then[4];
float cal_airspeed[4];
int analogInPin[4]; //The default software port for each pitot starts at A1 and then marches from there.
float airspeed[4];
float airspeed_now[4];
float timerCAL = 0;
int CAL_DONE = 0;

void setup() 
{
  //initialize serial communications at a 9600 baud rate
  Serial.begin(9600);
  for (int idx = 0;idx<numVars;idx++){
    data[idx] = 0;
  }
  digitalWrite(13,LOW);
  
  //For the 1 hole pitot sensor - Quad reference frame
  //A2 = North pitot probe 

  //For the 4 hole pitot sensor - Quad reference frame
  //A0 = West pitot probe
  //A1 = North Pitot Probe
  //A2 = South Pitot probe
  //A3 = East Pitot Probe

  //Thus columns on the SD card will be
  //West, North, South, East - Quad Reference frame

  //Note that typically I point the quad itself in the EAST Earth Coordinate direction
  //thus the SD card will actually have a different reference. However, to avoid confusion
  //I suggest you point the quad due north so that the pitot reference frame matches with the
  //Earth reference frame
  
  //Remember to verify this in the python code

  for (int idx = 0;idx<numPitot;idx++) {
    analogInPin[idx] = idx+1;
  }
  digitalWrite(13,LOW);
  //Read pitot sensor N times to calibrate sensor
  for (int idx = 0;idx<numPitot;idx++) {
      cal_voltage[idx] = 0;
      cal_airspeed[idx] = 0;
  }
  int N = 1000;
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
  for (int idx = 0;idx<N;idx++) {
    for (int jdx = 0;jdx<numPitot;jdx++) {
      float sensorvalue = analogRead(analogInPin[jdx]);
      //Convert to voltage
      float raw_voltage = sensorvalue*(5.0/1023.0);
      //Calibrate voltage
      float voltage = raw_voltage - cal_voltage[jdx];
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
      airspeed_now[jdx] = a_inf*(sqrt(5.0*(k-1.0)));
      airspeed[jdx] = airspeed_now[jdx];
      cal_airspeed[jdx]+=airspeed[jdx];
    }
  }
  for (int idx = 0;idx<numPitot;idx++) {
    cal_airspeed[idx] = cal_airspeed[idx]/float(N);
  }
  //timerCAL = millis()/1000.0;
}


void loop()
{
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)

  //Pull Data from Airspeed sensor
  //Serial.print(" Airspeed = ");
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
    //airspeed[idx] = airspeed_now[idx];
    //Use a complimentary filter to filter out noise
    float sigma = 0.03;
    airspeed[idx] = (1.0-sigma)*airspeed_then[idx] + sigma*airspeed_now[idx];
    airspeed_then[idx] = airspeed[idx];
    data[idx] = airspeed[idx]-cal_airspeed[idx];
    //Serial.print(" Raw Bits = ");
    //Serial.print(sensorvalue);
    //Serial.print(" Raw Voltage = ");
    //Serial.print(raw_voltage,8);
    //Serial.print(" Voltage = ");
    //Serial.print(voltage);
    //Serial.print(airspeed[idx]-cal_airspeed[idx]);
    //Serial.print(" ");
  }
  //Serial.print(" m/s \n");
  //Random Number
  //for (int idx = numPitot;idx<6;idx++){
  //   data[idx] = random(1,10);
  //}

//  //Re-calibrate
//  if (millis()/1000.0 > timerCAL + 10.0 && CAL_DONE == 0) {
//    //Serial.print("Recalibrating \n");
//    CAL_DONE = 1;
//    //Read pitot sensor N times to calibrate sensor
//    for (int idx = 0;idx<numPitot;idx++) {
//      cal_voltage[idx] = 0;
//      cal_airspeed[idx] = 0;
//    }
//    int N = 1000;
//    for (int idx = 0;idx<N;idx++) {
//      for (int jdx = 0;jdx<numPitot;jdx++){
//          sensorvalue = analogRead(analogInPin[jdx]);
//          cal_voltage[jdx]+=sensorvalue*(5.0/1023.0);
//        }
//    }
//    for (int idx = 0;idx<numPitot;idx++) {
//      cal_voltage[idx] = cal_voltage[idx]/float(N);
//    }
//    for (int idx = 0;idx<N;idx++) {
//      for (int jdx = 0;jdx<numPitot;jdx++){
//          sensorvalue = analogRead(analogInPin[jdx]);
//          float voltage = sensorvalue*(5.0/1023.0) - cal_voltage[jdx];
//          //Convert to pressure
//          float pressure = voltage/101.325;
//          //Compute Airspeed using bernoulli
//          float k = pow(pressure+1.0,2.0/7.0);
//          //Check for error
//          if (k < 1.0) {
//            k = 1.0;
//          }
//          float tempC = 20.0;
//          float tempK = 273.15 + tempC;
//          float a_inf = sqrt(1.4*286.0*tempK);
//          //Compute Airspeed
//          cal_airspeed[jdx] += a_inf*(sqrt(5.0*(k-1.0))); 
//        }
//    }
//    for (int idx = 0;idx<numPitot;idx++) {
//      cal_airspeed[idx] = cal_airspeed[idx]/float(N);
//      //Serial.print("Calibration Airspeed = ");
//      //Serial.println(cal_airspeed[idx]);   
//    }
//  }
  
  //Write Data to Serial
  serialPrintFloatArr(data,numVars);
  Serial.println("\r\n"); //line break. Tells processing to stop reading data

  //wait a bit so you don't write super freaking fast
  //delay(10);
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}

