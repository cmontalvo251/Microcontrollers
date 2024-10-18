import time
import board
import busio
from adafruit_crickit import crickit

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

##Set drive frequency
crickit.drive_1.frequency = 1000

while True:
    #Get time
    t = time.monotonic()

    ##Get for button press
    if buttonA.value == False:
        print('Detach')
        crickit.drive_1.fraction = 0.0
    else:
        crickit.drive_1.fraction = 1.0

    #Print and sleep
    print((t,))
    time.sleep(0.1)
