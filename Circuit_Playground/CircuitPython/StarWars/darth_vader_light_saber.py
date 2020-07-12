import audioio
import time
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio
import busio
import adafruit_lis3dh

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

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

def PLAY_SOUND(filename):
    print("Playing Sound")
    f = open(filename, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    while a.playing:
        pass
    f.close()
    return

class LightSaber():
    def __init__(self):
        print('Light Saber Created')
        PLAY_SOUND('fx4_audacity.wav')

##Right Button Press
def change_brightness():
    global brightness
    brightness+=10
    if brightness > 255:
        brightness = 5
    print(brightness)

###Create a light saber
darth = LightSaber()

clash = ['clash3_audacity.wav','clash_audacity.wav','clash2_audacity.wav','clash4_audacity.wav']
ctr = 0
while True:
    if buttonA.value:
        print("Button A pressed")
        change_brightness()
    if buttonB.value:
        print("Button B Pressed")
        PLAY_SOUND('darth.wav')
    color = (brightness,0,0)
    pixels.fill(color)
    pixels.show()

    x,y,z = lis3dh.acceleration
    val = abs(x) + abs(y) + abs(z)
    delta = abs(9.81 - val)
    print((delta,))
    if delta > 15.0:
        PLAY_SOUND(clash[ctr])
        ctr+=1
        if ctr > len(clash)-1:
            ctr = 0
    elif delta > 10.0:
        PLAY_SOUND('Hum4_audacity.wav')

    time.sleep(0.1)