import time
import neopixel
import board
pixels = neopixel.NeoPixel(board.D1,30,brightness=0.25)

pixels.fill((255,255,255))
pixels.show()

red = 0
blue = 0
green = 0
while True:
    print((red,blue,green))
    red+=20 
    blue-=13
    green+=17
    if red > 255:
        red = 0
    if blue < 0:
        blue = 255
    if green > 255:
        green = 0
    pixels.fill((red,blue,green))
    pixels.show()
    time.sleep(0.1)