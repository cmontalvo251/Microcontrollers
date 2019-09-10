from adafruit_circuitplayground.express import cpx
import math
import time

while True:
    x, y, z = cpx.acceleration
    print((z,))
    time.sleep(0.1)