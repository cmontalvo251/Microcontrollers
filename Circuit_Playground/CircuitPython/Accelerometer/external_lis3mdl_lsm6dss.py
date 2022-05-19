import time
import board
import busio
import digitalio
import adafruit_lis3dh
import adafruit_lis3mdl
from adafruit_lsm6ds.lsm6ds33 import LSM6DS33

i2c = busio.I2C(board.SCL, board.SDA)
accelgyro = LSM6DS33(i2c,address=0x6a)
mag = adafruit_lis3mdl.LIS3MDL(i2c)

##Accelerometer is hooked up to SDA/SCL which is I2C
i2cCPB = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2cCPB, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

#while not i2c_ext.try_lock():
#    pass

#try:
#    while True:
#        print("I2C addresses found:", [hex(device_address)
#              for device_address in i2c_ext.scan()])
#        time.sleep(2)

#finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
#    i2c_ext.unlock()
## 0x1e and 0x6a on my broken one
## 0x1c and 0x6a on a working one - 6a is the accelerometer/gyro and the 1e/1c is the magneometer


while True:
    axCPB,ayCPB,azCPB = lis3dh.acceleration
    bx,by,bz = mag.magnetic
    gx,gy,gz = accelgyro.gyro
    ax,ay,az = accelgyro.acceleration
    print(axCPB,ayCPB,azCPB,ax,ay,az,gx,gy,gz,bx,by,bz)
    time.sleep(0.1)