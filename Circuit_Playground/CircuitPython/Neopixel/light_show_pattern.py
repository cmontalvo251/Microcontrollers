import board
import neopixel
import time
import random as R

pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, auto_write=False)
pixels[0] = (10, 0, 0) ##rgb
pixels[9] = (0, 10, 0)
pixels.show()

while True:
    pixels.fill((0,0,0))
    pixels.show()
    for i in range(0,10):
        pixels.fill((0,0,0))
        pixels.show()
        pixels[i] = (R.randint(0,255),R.randint(0,255),R.randint(0,255))
        pixels.show()
        time.sleep(0.1)