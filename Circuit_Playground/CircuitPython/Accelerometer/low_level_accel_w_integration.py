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
vx = 0.0
x = 0.0
dt = 0.1
biasx = -0.276603
biasy = -0.133696
biasz = 9.787214
while time.monotonic() - starttime < 500:
    t = time.monotonic() - starttime
    ax,ay,az = lis3dh.acceleration
    axf = ax - biasx
    ayf = ay - biasy
    azf = az - biasz
    vx = vx + axf*dt
    x = x + vx*dt
    print((t,axf,ayf,azf,vx,x))
    time.sleep(dt)