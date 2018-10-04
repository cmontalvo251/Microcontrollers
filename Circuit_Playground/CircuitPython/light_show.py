import board
import neopixel
import time
import digitalio

def Light_Show():
    pixels.fill((255,255,255))
    pixels.show()
    Circle_Lights((0,255,0))
    Circle_Lights((255,0,0))
    Circle_Lights((0,0,255))
    End_Game()

def Circle_Lights(color):
    for idx in range(0,10):
        pixels[idx] = color
        pixels.show()
        time.sleep(0.1);
        pixels.fill((0,0,0))
        
def End_Game():
    for idx in range(0,4):
        pixels.fill((255,255,255))
        pixels.show()
        time.sleep(0.1)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.1)

##Setup globals
pixel_brightness = 0.25
color = (255,255,255)
MODE = 0
FLASH = 0
COLORMODE = 0

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

#Some helpful links
#https://learn.adafruit.com/adafruit-circuit-playground-express/circuitpython-digital-in-out
#http://circuitpython.readthedocs.io/en/2.x/shared-bindings/index.html

while True:
    Light_Show()