import time
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio
import busio
import adafruit_lis3dh

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

def PLAY_SOUND(filename):
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                Light_Show()
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
        time.sleep(0.05);
        pixels.fill((0,0,0))

def End_Game():
    for idx in range(0,4):
        pixels.fill((255,255,255))
        pixels.show()
        time.sleep(0.05)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.05)

##Right Button Press
def change_brightness():
    global brightness
    brightness+=10
    if brightness > 255:
        brightness = 5
    print(brightness)

ctr = 0
while True:
    if buttonA.value:
        print("Button A pressed")
        change_brightness()
    if buttonB.value:
        print("Button B Pressed")
        PLAY_SOUND('Feliz.wav')
    color = (brightness,0,0)
    pixels.fill(color)
    pixels.show()

    x,y,z = lis3dh.acceleration
    print((z,))
    if z < 0:
        PLAY_SOUND('feliz.wav')

    time.sleep(0.1)