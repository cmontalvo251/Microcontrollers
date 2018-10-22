#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FreeTouch.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_NeoPixel.h>

Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

#define BIG_LED 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(BIG_LED,OUTPUT);

  if (! qt_1.begin())  
    Serial.println("Failed to begin qt on pin A2");
}

void loop() {
  // put your main code here, to run repeatedly:
  int light_value = analogRead(A1);
  
  //if (qt_1.measure() > 700) {
  if (light_value < 80) {
  //delay(1000);
     digitalWrite(BIG_LED,HIGH);
  } else {
  //delay(1000);
     digitalWrite(BIG_LED,LOW);
  }
}
