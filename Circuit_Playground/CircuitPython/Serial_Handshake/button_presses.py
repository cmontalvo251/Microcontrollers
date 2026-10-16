import board
import digitalio
import time

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

while True:
    print(time.monotonic(),int(buttonA.value))
    time.sleep(0.1)