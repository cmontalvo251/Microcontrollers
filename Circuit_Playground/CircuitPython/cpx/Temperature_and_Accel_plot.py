import time
from adafruit_circuitplayground.express import cpx

timeout = time.monotonic() + 120
while True:
    test = 0
    x, y, z = cpx.acceleration
    Temp = cpx.temperature
    light = cpx.light
    Temp = Temp * 1.8 +32
    print(x, y, z,light,Temp)
    time.sleep(1)
    if time.monotonic() > timeout:
        break