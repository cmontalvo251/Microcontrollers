import time
from adafruit_circuitplayground.express import cpx

timeout = time.time() + 120
while True:
    test = 0
    x, y, z = cpx.acceleration
    Temp = cpx.temperature
    Temp = Temp * 1.8 +32
    print(x, y, z,Temp)
    time.sleep(1)
    if time.time() > timeout:
        break