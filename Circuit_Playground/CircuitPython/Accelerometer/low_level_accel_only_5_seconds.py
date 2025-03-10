import time
import board
import busio
import digitalio
import adafruit_lis3dh

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G
starttime = time.monotonic()
dt = 0.1
while time.monotonic() - starttime < 5:
    t = time.monotonic() - starttime
    ax,ay,az = lis3dh.acceleration
    print((t,ax,ay,az))
    time.sleep(dt)