import time
from adafruit_circuitplayground.express import cpx

while True:
    #print("Light:", cpx.light)
    val = cpx.light
    print((val,))
    time.sleep(0.1)