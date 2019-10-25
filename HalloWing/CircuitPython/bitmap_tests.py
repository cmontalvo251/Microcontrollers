import board
import digitalio
import time
import touchio
import audioio

def PokeHeal():
    SCREEN[-1] = displayio.TileGrid(BITMAPS[2],pixel_shader=displayio.ColorConverter(),x=0, y=0)
    time.sleep(1)
    SCREEN[-1] = displayio.TileGrid(BITMAPS[3],pixel_shader=displayio.ColorConverter(),x=0, y=0)

#Set up Touch Buttons
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

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

tstart = time.monotonic()
tnext = 1.0
ctr = 0

while True:
    if time.monotonic() > tstart + tnext:
        ctr+=1
        tstart = time.monotonic()
    if ctr == 2:
        ctr = 0
    SCREEN[-1] = displayio.TileGrid(BITMAPS[ctr],pixel_shader=displayio.ColorConverter(),x=0, y=0)
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
        PokeHeal()
    time.sleep(0.05)
    