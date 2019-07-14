#include <BME280_SIMPLE.h> //Uncommenting this leaves 461 bytes and code still works

Adafruit_BME280 bme; //Uncommenting this leaves 421 bytes and code still works


void setup() {

  Serial.begin(115200);

  bme.begin();

}

void loop() {

  //Pressure/Temp/Humidity
  //float temp = bme.readTemperature();
  //float pressure = bme.readPressure() / 100.0F;
  //float humidity = bme.readHumidity();

  Serial.print(bme.readTemperature()); Serial.print(" ");
  Serial.print(bme.readPressure() / 100.0F); Serial.print(" ");
  Serial.print(bme.readHumidity()); Serial.print(" ");
  Serial.println();
}

