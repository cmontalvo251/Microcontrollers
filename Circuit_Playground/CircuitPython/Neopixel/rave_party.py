import board
import neopixel
import time
import digitalio
import random as R

try:
    from audiocore import WaveFile
except ImportError:
    from audioio import WaveFile

try:
    from audioio import AudioOut
except ImportError:
    try:
        from audiopwmio import PWMAudioOut as AudioOut
    except ImportError:
        pass  # not always supported by every board!

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True

def PLAY_SOUND(filename):
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                pass
    return

##Setup globals
pixel_brightness = 0.25
color = (255,255,255)
MODE = 0
FLASH = 0
RAVE = 0
COLORMODE = 0

##Set up A button
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

##Set up B button
buttonB = digitalio.DigitalInOut(board.BUTTON_B)
buttonB.direction = digitalio.Direction.INPUT
buttonB.pull = digitalio.Pull.DOWN

##Set up slide
buttonS = digitalio.DigitalInOut(board.SLIDE_SWITCH)
buttonS.direction = digitalio.Direction.INPUT
buttonS.pull = digitalio.Pull.UP
VALUE = buttonS.value

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)
pixels.fill(color)
pixels.show()

#Some helpful links
#https://learn.adafruit.com/adafruit-circuit-playground-express/circuitpython-digital-in-out
#http://circuitpython.readthedocs.io/en/2.x/shared-bindings/index.html

##B Button Press
def buttonB_Pressed():
    global COLORMODE,color,RAVE
    COLORMODE+=1
    RAVE = 0
    if COLORMODE == 0:
        color = (255,255,255)
    elif COLORMODE == 1:
        color = (255,0,0)
    elif COLORMODE == 2:
        color = (0,255,0)
    elif COLORMODE == 3:
        color = (0,0,255)
    elif COLORMODE == 4:
        PLAY_SOUND('pony.wav')
        RAVE = 1
        COLORMODE = -1
    print(COLORMODE)
    time.sleep(0.2)

##A Button Press
def buttonA_Pressed():
    global MODE,FLASH,pixel_brightness,RAVE
    MODE+=1
    FLASH = 0
    RAVE = 0
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

    if RAVE:
        print('RAVE')
        time.sleep(0.2)
        color = (R.randint(0,255),R.randint(0,255),R.randint(0,255))