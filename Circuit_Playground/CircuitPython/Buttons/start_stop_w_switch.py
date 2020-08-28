import board
import digitalio
import time
#from adafruit_circuitplayground.express import cpx

##Button Presses
switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
x = 1

while True:
    x += 1
    time.sleep(0.1)
    print(switch.value)
    if switch.value:
        print(time.monotonic(),x)
        led.value = True
        time.sleep(0.5)
        led.value = False
        time.sleep(0.5)
        led.value = True
        time.sleep(0.1)
        led.value = False
        time.sleep(0.1)
