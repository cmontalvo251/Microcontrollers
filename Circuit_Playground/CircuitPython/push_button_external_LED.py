import board
import digitalio
import time

buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.A2)
led.direction = digitalio.Direction.OUTPUT
led.value = True

while True:
    print('Button value is ',buttonA.value)
    if buttonA.value == True:
        print('Button Value is ',buttonA.value)
        led.value = not led.value
        while buttonA.value == True:
            print('Waiting for you to let go....')
            # Wait for all buttons to be released.
            time.sleep(0.1)
    time.sleep(0.1)