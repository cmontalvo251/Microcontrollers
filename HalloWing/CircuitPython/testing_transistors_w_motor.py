import board
import digitalio
import time
import touchio
import audioio
from analogio import AnalogIn,AnalogOut

#Set up Touch Buttons
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

#print(dir(board))
#analog_button = AnalogOut(board.A6)

transistor = digitalio.DigitalInOut(board.D4)
transistor.direction = digitalio.Direction.OUTPUT
transistor.value = False

while True:
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
        transistor.value = 65535
        time.sleep(2)
        transistor.value = 0
    time.sleep(0.05)
    