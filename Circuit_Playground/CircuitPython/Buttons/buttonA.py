import board
import digitalio
import time

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

while True:
    time.sleep(0.1)
    print(time.monotonic(),buttonA.value)
    if buttonA.value:
        led.value = True
    else:
        led.value = False
