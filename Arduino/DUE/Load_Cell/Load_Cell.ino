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
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/

//HAVE NOT TESTED THIS ON A DUE YET.

#include "HX711.h"
//DOUT - A1
//CLK - A0
HX711 scale(A1, A0);
float zero_factor = 0;
void setup() {
  Serial.begin(38400);
  Serial.println("HX711 sketch");
  Serial.println("Remove all weight from scale");
  scale.read();
  scale.set_scale(2280.f);
  Serial.println("Calibration Factor Set");
  scale.tare(); //Reset the scale to 0
  Serial.println("Tare done");
  delay(1000);
  int count = 0;
  for (int idx = 0;idx<=10;idx++){
    Serial.print(idx);
    Serial.print(" ");
    Serial.println(zero_factor);
    float val = scale.get_units(1);
    if (abs(val) > 0 ) {
      zero_factor += val;
      count += 1;
    }
    delay(100);  
  }
  zero_factor/=count;
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}
void loop() {
  Serial.print("Reading (lbf): ");
  float weight_lbf = scale.get_units(1) - zero_factor;
  Serial.print(weight_lbf);
  Serial.print(" Time (sec): ");
  Serial.print(millis()/1000.0);
  Serial.print(" ");
  Serial.println();
  delay(100);
}
