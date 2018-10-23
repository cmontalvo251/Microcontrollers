  #include <Wire.h>
  #include <SPI.h>
  #include <Adafruit_LIS3DH.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_FreeTouch.h>
  #include <Adafruit_SPIFlash.h>
  #include <Adafruit_GFX.h>    // Core graphics library
  #include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
  #include <Adafruit_NeoPixel.h>
  //#include "squeaks_wav.h"
  
  Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
  
  //void play_tune(const uint8_t *audio, uint32_t audio_length);
  
  #define BIG_LED 3
  
  #include <Servo.h>
  
  #define SERVOPIN 4
  
  Servo mainservo;
  
  void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
  
    //Setup LEDs
    pinMode(BIG_LED,OUTPUT);
  
    //Turn on capacitive touch
    if (! qt_1.begin()) {
      Serial.println("Failed to begin qt on pin A2");
    }
  
    //Attach Servo
    mainservo.attach(SERVOPIN);
    mainservo.write(90);
  
    //Setup Audio
    //analogWriteResolution(10);
    //analogWrite(A0, 128);
      
  }
  
  void loop() {
    // put your main code here, to run repeatedly:
  
    ///Check for Touch
    if (qt_1.measure() > 700) {
      //play_tune(audio,sizeof(audio));
      //mainservo.write(0);
      //delay(1000);
      //mainservo.write(180);
      //delay(1000);
      //mainservo.write(90);
    }  
  
    //Check Light value
    if (analogRead(A1) < 80) {
       digitalWrite(BIG_LED,HIGH);
    } else {
       digitalWrite(BIG_LED,LOW);
    }
  
  }
  
  //Play tune audio
  //void play_tune(const uint8_t *audio, uint32_t audio_length) {
    //uint32_t t;
    //uint32_t prior, usec = 1000000L / SAMPLE_RATE;
    
    //for (uint32_t i=0; i<audio_length; i++) {
      //while((t = micros()) - prior < usec);
      //analogWrite(A0, (uint16_t)audio[i] / 8);
      //prior = t;
    //}
  //}
