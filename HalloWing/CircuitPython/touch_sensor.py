import board
import digitalio
import time
import touchio

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

#Set up Touch Buttons
print(dir(board))
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

while True:
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
    time.sleep(0.05)