import audioio
import time
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio
import busio
import digitalio
import adafruit_lis3dh

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
a = audioio.AudioOut(board.A0)

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, auto_write=False)

##Button Presses
buttonA = DigitalInOut(board.BUTTON_A)
buttonA.direction = Direction.INPUT
buttonA.pull = Pull.DOWN
buttonB = DigitalInOut(board.BUTTON_B)
buttonB.direction = Direction.INPUT
buttonB.pull = Pull.DOWN

brightness = 10

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

def check_brightness():
    global brightness
    if brightness > 255:
        brightness = 0
    if brightness < 0:
        brightness = 255
    print(brightness)
        
def change_brightness(increment):
    global brightness
    brightness+=increment
    check_brightness()

def PLAY_SOUND():
    print("Playing Sound")
    f = open("Pikachu.wav", "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    # You can now do all sorts of stuff here while the audio plays
    # such as move servos, motors, read sensors...
    # Or wait for the audio to finish playing:
    # Light_Show()
    # While the music is playing run the light show
    while a.playing:
        pass
    f.close()
    return
    
##Take a bunch of average readings
avg = 0
N = 100
for i in range(0,N):
    x,y,z = lis3dh.acceleration
    norm2 = x*x + y*y + z*z
    avg+=norm2
avg/=(N+1)
    
while True:
    if buttonA.value:
        change_brightness(10)
    if buttonB.value:
        change_brightness(-10)
    ###Top Row is red and bottom row is white
    pixels.fill((brightness,brightness,brightness))
    for i in range(0,5):
        pixels[i] = (brightness,0,0)
    pixels.show()
    time.sleep(0.1)
    x,y,z = lis3dh.acceleration
    norm2 = x*x + y*y + z*z
    if abs(norm2 - avg) > 100:
        PLAY_SOUND()
    #print((norm2-avg,))