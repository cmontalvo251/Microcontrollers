# Snow Globe
# for Adafruit Circuit Playground express
# with CircuitPython
import math
import time
import audioio
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio
import busio
import adafruit_lis3dh

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
a = audioio.AudioOut(board.A0)

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, auto_write=False,brightness=10)

ROLL_THRESHOLD = 0  # Total angle

WHITE = (65, 65, 65)
RED = (220, 0, 0)
GREEN = (0, 220, 0)
BLUE = (0, 0, 220)
SKYBLUE = (0, 20, 200)
BLACK = (0, 0, 0)

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

# Initialize the global states
new_roll = False
rolling = False

def fade_pixels(fade_color):  # pick from colors defined above, e.g., RED, GREEN, BLUE, WHITE, etc.
    # fade up
    for j in range(25):
        pixel_brightness = (j * 0.01)
        pixels.brightness = pixel_brightness
        for i in range(10):
            pixels[i] = fade_color

    # fade down
    for k in range(25):
        pixel_brightness = (0.25 - (k * 0.01))
        pixels.brightness = pixel_brightness
        for i in range(10):
            pixels[i] = fade_color

# fade in the pixels
#fade_pixels(GREEN)

def Light_Show():
    pixels.fill(WHITE)
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
        pixels.fill(WHITE)
        pixels.show() 
        time.sleep(0.01)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.01)


def PLAY_SOUND():
    print("Playing Sound")
    f = open("Feliz.wav", "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    ctr = 0
    while a.playing:
        ctr+=1
        if ctr < 8:
            Light_Show()    
        pass
    f.close()
    return

# Loop forever
while True:
    # check for shaking
    # Compute total acceleration
    x_total = 0
    y_total = 0
    z_total = 0
    for count in range(10):
        x, y, z = lis3dh.acceleration
        x_total = x_total + x
        y_total = y_total + y
        z_total = z_total + z
        time.sleep(0.001)
    x_total = x_total / 10
    y_total = y_total / 10
    z_total = z_total / 10

    total_angle = math.atan(math.sqrt(x_total**2 + y_total**2)/z_total)*180./3.14
    #print(total_angle)

    # Check for rolling
    if total_angle < ROLL_THRESHOLD:
        PLAY_SOUND()
        fade_pixels(GREEN)
        pixels.brightness = 10