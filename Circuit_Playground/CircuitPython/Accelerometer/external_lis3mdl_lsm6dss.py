import time
import board
import busio
import digitalio
#import adafruit_lis3dh
#from adafruit_lsm6ds.lsm6ds33 import LSM6DS33
import adafruit_lis3mdl

i2c_ext = busio.I2C(board.SCL, board.SDA)

#print(dir(adafruit_lsm6ds))
#sensor_ext = LSM6DS33(i2c_ext,address=0x6a)
mag = adafruit_lis3mdl.LIS3MDL(i2c_ext,address=0x1c)

##Accelerometer is hooked up to SDA/SCL which is I2C
#i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
#_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
#lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
#lis3dh.range = adafruit_lis3dh.RANGE_8_G

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
    #x,y,z = lis3dh.acceleration
    #xe,ye,ze = sensor_ext.acceleration
    #gx,gy,gz = sensor_ext.gyro
    bx,by,bz = mag.magnetic
    print((bx,by,bz))
    #print((x,xe,y,ye,z,ze))
    #print((x,y,z))
    #print((xe,ye,ze))
    #print((gx,gy,gz))
    time.sleep(0.1)