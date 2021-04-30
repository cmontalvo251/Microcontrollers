import board
import neopixel
import time
import digitalio

pixel_brightness = 0.25

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

#Some helpful links
#https://learn.adafruit.com/adafruit-circuit-playground-express/circuitpython-digital-in-out
#http://circuitpython.readthedocs.io/en/2.x/shared-bindings/index.html
b = 40
while True:
    pixels.fill((b,b,b)) #this will make all lights white
    time.sleep(2)
    pixels.fill((0,0,0))
    pixels[0] = (b,0,0) #this will make the first pixel red
    time.sleep(2)
    pixels.fill((0,0,0))