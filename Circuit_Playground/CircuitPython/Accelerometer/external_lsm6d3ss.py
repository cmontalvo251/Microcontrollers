import time
import board
import busio
import digitalio
import adafruit_lis3dh
from adafruit_lsm6ds.lsm6ds33 import LSM6DS33

i2c_ext = busio.I2C(board.SCL, board.SDA)

#print(dir(adafruit_lsm6ds))
sensor_ext = LSM6DS33(i2c_ext)

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

while True:
    x,y,z = lis3dh.acceleration
    xe,ye,ze = sensor_ext.acceleration
    gx,gy,gz = sensor_ext.gyro
    #print((x,xe,y,ye,z,ze))
    print((gx,gy,gz))
    time.sleep(0.1)