import time
import board
import digitalio
from adafruit_circuitplayground.express import cpx
import os


if cpx.switch == False:
    file = open('Method3.txt','w')
else:
    print('Not opening file for writing')

start_time = time.monotonic()

while time.monotonic()-start_time < 60:
    #cpx.play_tone(440,0.3)
    cpx.led = True
    light = cpx.light
    temp = cpx.temperature
    x,y,z = cpx.acceleration
    print(time.monotonic(),light,temp,x,y,z)
    if cpx.switch == False:
        #cpx.play_tone(262,0.3)
        print('Writing Data to Disk')
        output = str(time.monotonic()) + " " + str(light) + " " + str(temp) + " " + str(x) + " " + str(y) + " " +str(z) + str('\n')
        file.write(output)
        file.flush()
        cpx.led = False
    else:
        print('Not logging data. Flip the switch and then hit reset')
    time.sleep(1) #sleep for so many seconds between measurements