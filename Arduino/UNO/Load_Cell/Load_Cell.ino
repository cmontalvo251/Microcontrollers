/*
<br> Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).
 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library:  https://github.com/bogde/HX711

 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 //Remember to plug these into DIGITAL pins!!!!
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/
#include "HX711.h"
#define DOUT  3
#define CLK  2
HX711 scale(DOUT, CLK);
float zero_factor = 0;
float calibration_factor = -28000; //-7050 worked for my 440lb max scale setup
char val[15]; //Data received from the serial port
char throttle_str[5]; //4 digit throttle command sent to motors in string
void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  scale.set_scale(calibration_factor);
  scale.tare(); //Reset the scale to 0
  for (int idx = 0;idx<100;idx++){
    Serial.println(idx);
    zero_factor += (scale.get_units()/2.2)*1000.; //Get a baseline reading  
  }
  zero_factor/=100.;
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}
void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  //Serial.print("Reading: ");
  Serial.print((scale.get_units()/2.2)*1000.0-zero_factor, 1);
  Serial.print(" ");
  //Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  //Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.print(" ");
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.println();
  //Check For Inputs from the User
  ///////////////////////////////
  if (Serial.available() > 0) {
    for (int idx = 0;idx<6;idx++){
      val[idx] = '0';
    }
    val[0] = '9';
    int length_of_str = Serial.available();
    int ctr = 0;
    //Read the contents of the serial command
    while (Serial.available()) {
      val[ctr] = Serial.read();
      ctr++;
      delay(10);
    }
    if (length_of_str == 5) {
      //This means we're changing the nominal throttle
      char throttle_str[5];
      for (int idx = 0;idx<5;idx++) {
        throttle_str[idx] = val[idx];
      }
      calibration_factor = -atoi(throttle_str);
    }
  }
}
