/**** 
 * Basic demo for Hallowing 
 * Displays SPI flash details, accelerometer readings, light reading,
 *  external sensor port reading, and capacitive touch sensing on the 
 *  four 'teeth'
 * At the same time, if a tap is detected, audio will play from the speaker
 * And, any NeoPixels connected on the NeoPixel port will rainbow swirl
 */


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FreeTouch.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_NeoPixel.h>
#include "audio.h"

#define TFT_CS     39
#define TFT_RST    37
#define TFT_DC     38
#define TFT_BACKLIGHT 7

#define FLASHCS   SS1
Adafruit_SPIFlash flash(FLASHCS, &SPI1);

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_2 = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_3 = Adafruit_FreeTouch(A4, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_4 = Adafruit_FreeTouch(A5, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// 30 LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, 4, NEO_GRB + NEO_KHZ800);

void play_tune(const uint8_t *audio, uint32_t audio_length);

void setup(void) {
  Serial.begin(9600);
  //while (!Serial);

  Serial.println("Hallowing test!");

  // Start TFT and fill black
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);

  // Turn on backlight
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextWrap(true);

  flash.begin(SPIFLASHTYPE_W25Q16BV);

  uint8_t manid, devid;
  Serial.println("Reading Manuf ID");
  flash.GetManufacturerInfo(&manid, &devid);
  Serial.print("JEDEC ID: 0x"); Serial.println(flash.GetJEDECID(), HEX);
  Serial.print("Manuf: 0x"); Serial.print(manid, HEX);
  Serial.print(" Device: 0x"); Serial.println(devid, HEX);

  tft.print("SPI Flash OK\nManuf: 0x"); tft.print(manid, HEX);
  tft.print(" Dev: 0x"); tft.println(devid, HEX);
  
  if (! lis.begin(0x18) && ! lis.begin(0x19)) {
    Serial.println("Couldnt start lis3dh");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  lis.setClick(1, 80);
  
  if (! qt_1.begin())  
    Serial.println("Failed to begin qt on pin A2");
  if (! qt_2.begin())  
    Serial.println("Failed to begin qt on pin A3");
  if (! qt_3.begin())  
    Serial.println("Failed to begin qt on pin A4");
  if (! qt_4.begin())  
    Serial.println("Failed to begin qt on pin A5");

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  analogWriteResolution(10);
  analogWrite(A0, 128);
}

uint8_t j = 0;

void loop() {
  uint8_t click = lis.getClick();
  if (click & 0x30) {
    Serial.print("Click detected (0x"); Serial.print(click, HEX); Serial.print("): ");
    if (click & 0x10) Serial.print(" single click");
    if (click & 0x20) Serial.print(" double click");
    play_tune(audio, sizeof(audio));
  }

  sensors_event_t event; 
  lis.getEvent(&event);
  
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Accel: \t X: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
  Serial.println(" m/s^2 ");

  tft.fillRect(0, 20, 128, 8, ST77XX_BLACK);
  tft.setCursor(0, 20);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("X:"); tft.print(event.acceleration.x, 1);
  tft.print(" Y:"); tft.print(event.acceleration.y, 1);
  tft.print(" Z:"); tft.print(event.acceleration.z, 1);

  // Read light sensor
  Serial.print("Light: "); Serial.println(analogRead(A1));

  tft.fillRect(0, 30, 128, 8, ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Light: "); tft.println(analogRead(A1));

  tft.fillRect(0, 40, 128, 8, ST77XX_BLACK);
  tft.setCursor(0, 40);
  tft.setTextColor(ST77XX_RED);
  float vbat = analogRead(A6)*2*3.3/1024;
  Serial.print("Battery: "); Serial.print(vbat); Serial.println("V");
  tft.print("Battery: "); tft.print(vbat); tft.println("V");


  tft.fillRect(0, 50, 128, 8, ST77XX_BLACK);
  tft.setCursor(0, 50);
  tft.setTextColor(ST77XX_BLUE);
  float vsense = analogRead(A11)*3.3/1024;
  Serial.print("Sensor: "); Serial.print(vsense); Serial.println("V");
  tft.print("Sensor: "); tft.print(vsense); tft.println("V");


  Serial.print("QT 1: "); Serial.print(qt_1.measure());
  Serial.print("\tQT 2: "); Serial.print(qt_2.measure());
  Serial.print("\tQT 3: "); Serial.print(qt_3.measure());
  Serial.print("\tQT 4: "); Serial.println(qt_4.measure());

  if (qt_4.measure() > 700) {
     tft.fillCircle(16, 110, 10, ST77XX_BLUE);
  } else {
     tft.fillCircle(16, 110, 10, ST77XX_BLACK);
  }
  if (qt_3.measure() > 700) {
     tft.fillCircle(48, 110, 10, ST77XX_BLUE);
  } else {
     tft.fillCircle(48, 110, 10, ST77XX_BLACK);
  }
  if (qt_2.measure() > 700) {
     tft.fillCircle(80, 110, 10, ST77XX_BLUE);
  } else {
     tft.fillCircle(80, 110, 10, ST77XX_BLACK);
  }
  if (qt_1.measure() > 700) {
     tft.fillCircle(112, 110, 10, ST77XX_BLUE);
  } else {
     tft.fillCircle(112, 110, 10, ST77XX_BLACK);
  }

  
  for(int32_t i=0; i< strip.numPixels(); i++) {
     strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j*5) & 255));
  }
  j++;
  strip.show();
}

void play_tune(const uint8_t *audio, uint32_t audio_length) {
  uint32_t t;
  uint32_t prior, usec = 1000000L / SAMPLE_RATE;
  
  for (uint32_t i=0; i<audio_length; i++) {
    while((t = micros()) - prior < usec);
    analogWrite(A0, (uint16_t)audio[i] / 8);
    prior = t;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

