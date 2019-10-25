import board
import digitalio
import time
from analogio import AnalogIn

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT

#Rear Led
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.A1)
threshold = 30.0

while True:
    light = photocell.value*330/(2**16)
    print((light,))
    if light < threshold:
        big_led.value = True
    else:
        big_led.value = False
            
    