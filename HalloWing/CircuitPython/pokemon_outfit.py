import board
import digitalio
import time
import touchio
import audioio
from analogio import AnalogIn,AnalogOut

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

a = audioio.AudioOut(board.SPEAKER) #this is equivalent to board.A0

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
board.DISPLAY.brightness = 1.0

def HealPokemon():
    SCREEN[-1] = displayio.TileGrid(BITMAPS[3],pixel_shader=displayio.ColorConverter(),x=0, y=0)
    wavfile = "poke_center.wav"
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    tstart = time.monotonic()
    tnext = 0.25
    while a.playing:
        #pass
        #print("Playing")
        if time.monotonic() > tstart + tnext:
            tstart += tnext
            big_led.value = not big_led.value
    big_led.value = False
    f.close()

#Setup Button Activation
analog_button = AnalogIn(board.A2)

#Setup counter for bitmap swaps
tstart = time.monotonic()
tnext = 2.0
ctr = 0

while True:
    if time.monotonic() > tstart + tnext:
        ctr+=1
        tstart = time.monotonic()
        if ctr == 2:
            ctr = 0
        SCREEN[-1] = displayio.TileGrid(BITMAPS[ctr],pixel_shader=displayio.ColorConverter(),x=0, y=0)
    #print(analog_button.value)
    if analog_button.value > 32000:
        #print("Touched")
        HealPokemon()
    time.sleep(0.05)
    