from adafruit_circuitplayground.express import cpx
import time

#print(dir(cpx._temp))

while True:
    light = cpx.light
    #light = cpx._light._photocell.value #if you want analog signal
    temp = cpx.temperature
    #temp = cpx._temp.pin.value #if you want analog
    x,y,z = cpx.acceleration
    timenow = time.monotonic()
    print(timenow,light,temp,x,y,z)
    time.sleep(0.5)