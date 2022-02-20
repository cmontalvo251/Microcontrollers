import board
import neopixel
import time
import digitalio

##Setup globals
pixel_brightness = 1.0
colors = [(255,0,0),(255,255,0),(255,80,0)]
COLORINDEX = 0
FADE = -1
SPEED = 0.03

##Set up A button
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

##Set up B button
buttonB = digitalio.DigitalInOut(board.BUTTON_B)
buttonB.direction = digitalio.Direction.INPUT
buttonB.pull = digitalio.Pull.DOWN

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)
pixels.fill(colors[COLORINDEX])
pixels.show()

#Some helpful links
#https://learn.adafruit.com/adafruit-circuit-playground-express/circuitpython-digital-in-out
#http://circuitpython.readthedocs.io/en/2.x/shared-bindings/index.html

while True:
    #Reset color and brightness
    pixels.brightness = pixel_brightness
    pixels.fill(colors[COLORINDEX])
    pixels.show()
    time.sleep(0.01)
    pixel_brightness+=FADE*SPEED
    if pixel_brightness < 0 and FADE == -1:
        FADE = 1
        COLORINDEX += 1
        if COLORINDEX >= len(colors):
            COLORINDEX = 0
        print(COLORINDEX)
    if pixel_brightness > 1 and FADE == 1:
        FADE = -1