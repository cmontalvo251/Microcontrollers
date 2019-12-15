import time
import board
import busio
import digitalio
import adafruit_lis3dh
import math
import pulseio
import neopixel

# Initialize PWM output for the servo (on pin A2):
servo = pulseio.PWMOut(board.A2, frequency=50)

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

#Initialize Neopixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=10,auto_write=True)

while True:
    #Accelerometer
    x,y,z = lis3dh.acceleration
    #If the USB port is pointed along the y-body axis
    #z is down and x is forward
    theta_deg = -math.atan(x/z)*180./3.14



    #Get forward and rear lights
    #theta = 90, fl = 0
    #theta = -90, fl = 255
    #slope = -255/180
    m = 100.
    r = 45.
    fl = int(-m/(2*r)*(theta_deg-r))
    if fl < 0:
        fl = 0
    if fl > 255:
        fl = 255
    #theta = 90, rl = 255
    #theta = -90, rl = 0
    #slope = 255/180
    rl = int(m/(2*r)*(theta_deg-r) + m)
    if rl < 0:
        rl = 0
    if rl > 255:
        rl = 255

    for i in range(0,5):
        pixels[i] = (fl,fl,fl)
    for i in range(5,10):
        pixels[i] = (rl,rl,rl)

    print((theta_deg,fl,rl))
    time.sleep(0.1)
