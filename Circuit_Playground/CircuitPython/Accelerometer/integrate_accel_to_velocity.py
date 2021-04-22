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

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

##Measure acceleration 10 times to get rid of initial spike
for _ in range(10):
    ax,ay,az = lis3dh.acceleration

##Initiliaze Velocity
vz = 0.0
##Initialize Timestep
dt = 0.1
##Filter amount
s = 0.75 ##closer to 1 is more filtering
#Initial Filtered value
azf = 0.0
while True:
    ##Obtain raw measurement
    ax,ay,az = lis3dh.acceleration
    ##Remove Bias in az
    az += (9.81-0.46) #Change to whatever works for your sensor
    ###Filter signal
    azf = azf*s + (1-s)*az
    ##Now integrate just using a reimann sum
    vz += azf*dt
    ##Use a button press to reset speed
    if buttonA.value == True:
        vz = 0.0
    print((az,azf,vz))
    time.sleep(dt)

