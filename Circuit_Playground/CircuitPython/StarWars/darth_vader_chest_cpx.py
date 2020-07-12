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
brightness = 255
color = (brightness,0,0)
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
def change_brightness():
    global brightness
    brightness+=10
    if brightness > 255:
        brightness = 5
    print(brightness)

while True:
    if buttonA.value:
        print("Button A pressed")
        change_brightness()
    if buttonB.value:
        print("Button B Pressed")
        PLAY_SOUND()
    color = (brightness,0,0)
    pixels.fill(color)
    pixels.show()
    time.sleep(0.1)