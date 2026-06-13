// Basic demo for accelerometer/gyro readings from Adafruit LSM6DS33
#include <Adafruit_LSM6DS33.h>
Adafruit_LSM6DS33 lsm6ds33;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Adafruit LSM6DS33 test!");

  if (!lsm6ds33.begin_I2C()) {
    Serial.println("Failed to find LSM6DS33 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("LSM6DS33 Found!");

  //Set ranges and values
  // lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G); //2/4/8/16
  // lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS); //125/250/500/1000/2000/4000
  // lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  // lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ); //12_5/26/52/104/208/416/833/1_66K/3_33K/6_66K
  lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2
}

void loop() {
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);

  Serial.print("Temperature ");
  Serial.print(temp.temperature);
  Serial.print(" deg C ");

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Accel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" Z: ");
  Serial.print(accel.acceleration.z);
  Serial.print(" m/s^2 ");

  /* Display the results (rotation is measured in rad/s) */
  Serial.print(" Gyro X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" Y: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" Z: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" rad/s ");
  Serial.println();

  delay(100);
}
