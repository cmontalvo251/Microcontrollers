import time
import board
from analogio import AnalogIn
import neopixel

analogin = AnalogIn(board.A6)
analogin2 = AnalogIn(board.A5)
analogin3 = AnalogIn(board.A2)

#Set up pixels
pixels = neopixel.NeoPixel(board.A3, 35, brightness=0.25)
pixels.fill((255,255,255))
pixels.show()

red = 0
blue = 0
green = 0

def saturate(input):
    out = input
    if out < 0:
        out = 0
    if out > 255:
        out = 255
    return out

while True:
    red = (analogin.value - 200)*255/65535
    blue = (analogin2.value - 200)*255/65535
    green = (analogin3.value - 200)*255/65535
    red = saturate(red)
    blue = saturate(blue)
    green = saturate(green)
    print((red,blue,green))
    pixels.fill((red,blue,green))
    pixels.show()
    time.sleep(0.1)
