import time
from adafruit_circuitplayground.express import cpx
import board
import digitalio

if not cpx.switch:
    file = open('Test_Data.txt','w')

timeout = time.time() + 3600
while True:
    cpx.red_led = True
    light = cpx.light
    Temp = cpx.temperature
    Temp = Temp * 1.8 +32
    print(time.monotonic(),light,Temp)
    if not cpx.switch:
        output = str(time.monotonic()) + " " + str(light) + " " + str(Temp) + str('\n')
        file.write(output)
        file.flush()
    cpx.red_led = False
    time.sleep(1)
    if time.time() > timeout:
        break