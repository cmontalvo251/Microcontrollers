import board
import digitalio
import time

GO = False

def checkButtonA():
    global GO
    if buttonA.value:
        print('A button pressed')
        GO = not GO
        time.sleep(0.5)

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
x = 1

while True:
    x += 1
    checkButtonA()
    if GO:
        print(time.monotonic(),x)
        led.value = True
        time.sleep(0.5)
        led.value = False
        time.sleep(0.5)
        led.value = True
        time.sleep(0.1)
        led.value = False
        time.sleep(0.1)
