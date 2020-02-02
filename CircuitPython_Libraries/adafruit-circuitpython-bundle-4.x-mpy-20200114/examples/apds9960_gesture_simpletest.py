from board import SCL, SDA
import busio
from adafruit_apds9960.apds9960 import APDS9960

i2c = busio.I2C(SCL, SDA)

apds = APDS9960(i2c)
apds.enable_proximity = True
apds.enable_gesture = True

while True:
    gesture = apds.gesture()

    if gesture == 0x01:
        print("up")
    elif gesture == 0x02:
        print("down")
    elif gesture == 0x03:
        print("left")
    elif gesture == 0x04:
        print("right")
