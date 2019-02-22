import audioio
import time
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.25)
##Button Presses
buttonA = DigitalInOut(board.BUTTON_A)
buttonA.direction = Direction.INPUT
buttonA.pull = Pull.DOWN
buttonB = DigitalInOut(board.BUTTON_B)
buttonB.direction = Direction.INPUT
buttonB.pull = Pull.DOWN
COLORMODE = 0
color = (255,255,255)
RANDOMCOLORS = False
a = audioio.AudioOut(board.A0)

def PLAY_SOUND():
    print("Playing Sound")
    f = open("darth.wav", "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    # You can now do all sorts of stuff here while the audio plays
    # such as move servos, motors, read sensors...
    # Or wait for the audio to finish playing:
    Light_Show()
    # While the music is playing run the light show
    while a.playing:
        pass
    f.close()
    return
    
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
        time.sleep(0.01);
        pixels.fill((0,0,0))
        
def End_Game():
    for idx in range(0,4):
        pixels.fill((255,255,255))
        pixels.show() 
        time.sleep(0.01)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.01)
    
##Right Button Press
def change_color():
    global COLORMODE,color,RANDOMCOLORS
    COLORMODE+=1
    if COLORMODE == 0:
        RANDOMCOLORS = False
        color = (255,255,255)
    elif COLORMODE == 1:
        color = (255,0,0)
    elif COLORMODE == 2:
        color = (0,255,0)
    elif COLORMODE == 3:
        color = (0,0,255)
    elif COLORMODE == 4:
        RANDOMCOLORS = True
        COLORMODE = -1

red = 0
green = 0
blue = 0
while True:
    if buttonA.value:
        print("Button A pressed")
        change_color()
    if buttonB.value:
        print("Button B Pressed")
        PLAY_SOUND()
    if RANDOMCOLORS == True:
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
        color = (red,green,blue)
    pixels.fill(color)
    pixels.show()
    time.sleep(0.1)