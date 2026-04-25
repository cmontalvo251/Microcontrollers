#Import Stuff
from random import randrange
import random
import board
import busio
import digitalio
import time
from digitalio import DigitalInOut, Direction, Pull

##Main Board LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

##Pixels
import neopixel
import adafruit_fancyled.adafruit_fancyled as fancy

##Button Presses
buttonA = DigitalInOut(board.BUTTON_A)
buttonA.direction = Direction.INPUT
buttonA.pull = Pull.DOWN
buttonB = DigitalInOut(board.BUTTON_B)
buttonB.direction = Direction.INPUT
buttonB.pull = Pull.DOWN

##Setup Pixels on board
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=255,auto_write=False)

def check_brightness():
    global brightness
    max_brightness = 255
    if brightness > max_brightness:
        brightness = 0
    if brightness < 0:
        brightness = max_brightness
    print("Current Brightness = ",brightness)

def change_brightness(increment):
    print("Changing Brightness by = ",increment)
    global brightness
    brightness+=increment
    check_brightness()

def CHANGE_MODE():
    global MODE
    MODE+=1
    if MODE > 4:
        MODE = 0
    print("Current MODE = ",MODE)

def get_color(now_time):
    global MODE,XMAS,change_time
    color = [(brightness,brightness,brightness)]*10
    if MODE == 0:
        color = [(brightness,0,0)]*10
    if MODE == 1:
        color = [(0,brightness,0)]*10
    if MODE == 2:
        color = [(0,0,brightness)]*10
    if MODE == 3:
        for i in range(0,10):
            r = random.randint(0,255)
            g = random.randint(0,255)
            b = random.randint(0,255)
            norm = (r*r + g*g + b*b)**(0.5)
            #print(r/norm,g/norm,b/norm,norm)
            rscale = int(r/norm*brightness)
            gscale = int(g/norm*brightness)
            bscale = int(b/norm*brightness)
            color[i] = (rscale,gscale,bscale)
    #print(color)
    if MODE == 4:
        if now_time - change_time > 2:
            XMAS = not XMAS
            change_time = now_time
        if XMAS:
            color_main = [(brightness,0,0)]*10
            color_alt = (0,brightness,0)
        else:
            color_main = [(0,brightness,0)]*10
            color_alt = (brightness,0,0)
        #print(color_main)
    else:
        color_main = color
        color_alt = color
    return color_main,color_alt

#Globals
MODE = 3
sparkle_start = 0
change_time = 0
SPARKLE = False
XMAS = True
brightness = 255
color = get_color(0)
FLASH = True

while True:
    #Get Current Time for Event Handling
    now_time = time.monotonic()
    #print("Time = ",now_time)

    #Button A event
    if buttonA.value:
        CHANGE_MODE()
        time.sleep(0.25)

    #Capacitive Touch Event
    if buttonB.value:
        change_brightness(1)

    ##Now we actually do stuff based on the following settings
    #brightness
    #SPARKLE -- tells us if we need to be sparkling
    #SEQUINS -- tells us if sequins are always on
    #MODE -- which mode we are in (solid red,blue, green, random, red/green duty cycle)

    #Alright let's do pixels on the CPX first
    color_main,color_alt = get_color(now_time)
    for i in range(0,10):
        pixels[i] = color_main[i]
    pixels.show()

    #Slow Down Sim
    time.sleep(0.1)