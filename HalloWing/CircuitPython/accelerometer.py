import board
import digitalio
import time
from analogio import AnalogIn,AnalogOut
import busio
import adafruit_lis3dh

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT

#Rear Led
#led = digitalio.DigitalInOut(board.D13)
#led.direction = digitalio.Direction.OUTPUT

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
print(dir(board))
i2c = busio.I2C(board.SCL, board.SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x18, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.A1)
while True:
    light = photocell.value*330/(2**16)
    x,y,z = lis3dh.acceleration
    print("Time (sec) = ",time.monotonic(),"Light (Lux) =",light,"Accel (m/s^2) = ",x,y,z)
    time.sleep(0.1)
    if x < 0:
        big_led.value = True
    else:
        big_led.value = False
    