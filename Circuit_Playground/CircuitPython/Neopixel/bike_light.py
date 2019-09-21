import board
import neopixel
import time
import digitalio

##Setup globals
pixel_brightness = 0.25
color = (255,255,255)
MODE = 0
FLASH = 0
COLORMODE = 0

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
pixels.fill(color)
pixels.show()

#Some helpful links
#https://learn.adafruit.com/adafruit-circuit-playground-express/circuitpython-digital-in-out
#http://circuitpython.readthedocs.io/en/2.x/shared-bindings/index.html

##B Button Press
def buttonB_Pressed():
    global COLORMODE,color
    COLORMODE+=1
    if COLORMODE == 0:
        color = (255,255,255)
    elif COLORMODE == 1:
        color = (255,0,0)
    elif COLORMODE == 2:
        color = (0,255,0)
    elif COLORMODE == 3:
        color = (0,0,255)
        COLORMODE = -1
    time.sleep(0.2)

##A Button Press
def buttonA_Pressed():
    global MODE,FLASH,pixel_brightness
    MODE+=1
    FLASH = 0
    if MODE == 0:
        pixel_brightness = 0.25
    elif MODE == 1:
        pixel_brightness = 0.02
    elif MODE == 2:
        pixel_brightness = 0.25
        FLASH = 1
    else:
        pixel_brightness = 0
        MODE = -1
    time.sleep(0.2)

while True:
    #Check for Button Presses
    if buttonA.value == True:
        buttonA_Pressed()
    if buttonB.value == True:
        buttonB_Pressed()
    #Reset color and brightness
    pixels.brightness = pixel_brightness
    pixels.fill(color)
    pixels.show()

    # If you want to change one pixel do this
    # pixels[0] = (0,255,0)
    # pixels.write()
    
    if FLASH:
        time.sleep(0.1)
        pixels.brightness = 0
        pixels.show()
        time.sleep(0.1)

        
