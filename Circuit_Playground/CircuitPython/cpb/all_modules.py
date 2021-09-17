from adafruit_circuitplayground.bluefruit import cpb
import time
print(dir(cpb))
starttime = time.monotonic()
while time.monotonic()-starttime < 5:
    t = time.monotonic()-starttime
    x,y,z = cpb.acceleration
    light = cpb.light
    sound_level = cpb.sound_level
    temperature = cpb.temperature
    cpb.red_led = True
    print((t,x,y,z,light,sound_level,temperature))
    time.sleep(0.1)
    cpb.red_led = False
    time.sleep(0.1)