import board
import digitalio
import time
import audioio
import busio
import adafruit_lis3dh
from analogio import AnalogIn,AnalogOut
import neopixel
from random import randint

##Neopixels
pixels = neopixel.NeoPixel(board.D4,30,brightness=1.0)

a = audioio.AudioOut(board.SPEAKER) #this is equivalent to board.A0

i2c = busio.I2C(board.SCL, board.SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x18, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

#Setup Images
FILES = ['psyduck.bmp','psyduck2.bmp','pokeball.bmp','pokeball2.bmp']       # BMP image to display - Use Gimp to save the bmp and select 16 bit R5 G6 B5
import displayio
board.DISPLAY.brightness = 0
SCREEN = displayio.Group()
board.DISPLAY.show(SCREEN)
BITMAPS = []
for file in FILES:
    BITMAPS.append(displayio.OnDiskBitmap(open(file, 'rb')))
SCREEN.append(displayio.TileGrid(BITMAPS[0],pixel_shader=displayio.ColorConverter(),x=0, y=0))
board.DISPLAY.brightness = 0.5

def Color_Wheel():
    red = randint(0,255)
    blue = randint(0,255)
    green = randint(0,255)
    for i in range(0,30):
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
        pixels.fill((red,blue,green))
        time.sleep(0.1)
        pixels.show()

def HealPokemon():
    SCREEN[-1] = displayio.TileGrid(BITMAPS[3],pixel_shader=displayio.ColorConverter(),x=0, y=0)
    wavfile = "poke_center.wav"
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    while a.playing:
        pass
    f.close()
    SCREEN[-1] = displayio.TileGrid(BITMAPS[0],pixel_shader=displayio.ColorConverter(),x=0, y=0)

#Setup Button Activation
touch_sensitive = AnalogIn(board.A5)

COLORMODE = -1

def change_color():
    global COLORMODE
    COLORMODE+=1
    if COLORMODE == 0:
        color = (255,0,0)
    elif COLORMODE == 1:
        color = (0,255,0)
    elif COLORMODE == 2:
        color = (0,0,255)
        COLORMODE = -1
    return color

time_last = time.monotonic()
while True:
    x,y,z = lis3dh.acceleration
    normxyz = x**2 + y**2 + z**2
    print(normxyz,touch_sensitive.value)
    if time.monotonic() - time_last > 2.0:
        time_last = time.monotonic()
        color = change_color()
        pixels.fill(color)
        pixels.show()
    if touch_sensitive.value < 20000:
        HealPokemon()
    if normxyz > 100:
        Color_Wheel()
    time.sleep(0.1)
