#Import Stuff
from random import randrange
import board
import busio
import displayio
import time
from adafruit_gizmo import tft_gizmo
import adafruit_imageload
import adafruit_lis3dh

#---| User Config |---------------
#BACKGROUND = 0x0FFF00              # specify color or background BMP file
#BACKGROUND = 'blinka_dark.bmp'
BACKGROUND = 'psyduck_240.bmp'
NUM_FLAKES = 50                    # total number of snowflakes
SNOW_COLOR = 0xFFFFFF              # snow color
SHAKE_THRESHOLD = 27               # shake sensitivity, lower=more sensitive
#---| User Config |---------------

# Accelerometer setup
accelo_i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
accelo = adafruit_lis3dh.LIS3DH_I2C(accelo_i2c, address=0x19)

# Create the TFT Gizmo display
display = tft_gizmo.TFT_Gizmo()

# Load background image
try:
    bg_bitmap, bg_palette = adafruit_imageload.load(BACKGROUND,bitmap=displayio.Bitmap,palette=displayio.Palette)
# Or just use solid color
except (OSError, TypeError):
    BACKGROUND = BACKGROUND if isinstance(BACKGROUND, int) else 0x000000
    bg_bitmap = displayio.Bitmap(display.width, display.height, 1)
    bg_palette = displayio.Palette(1)
    bg_palette[0] = BACKGROUND
background = displayio.TileGrid(bg_bitmap, pixel_shader=bg_palette)

# Shared palette for snow bitmaps
palette = displayio.Palette(2)
palette[0] = 0xADAF00   # transparent color
palette[1] = SNOW_COLOR # snow color
palette.make_transparent(0)

# Snowflake setup
# These are 4x4 grids for the snow flakes
FLAKES = (
    0, 0, 0, 0,    0, 0, 0, 0,    1, 1, 1, 1,
    0, 0, 0, 0,    1, 1, 1, 0,    1, 1, 1, 1,
    0, 1, 1, 0,    1, 1, 1, 0,    1, 1, 1, 1,
    0, 1, 1, 0,    1, 1, 1, 0,    1, 1, 1, 1,
)
##This sets up the flake types (4x4 times 3 is a 12x4 grid)
flake_sheet = displayio.Bitmap(12, 4, len(palette))
##Enumerate enumerates through the FLAKES storing the value and the counter
for i, value in enumerate(FLAKES):
    flake_sheet[i] = value
#and position
flake_pos = [0.0] * NUM_FLAKES
flakes = displayio.Group(max_size=NUM_FLAKES)
##This appends the flake sheet over and over again depending on the number of flakes.
#The _ is basically just a dummy variable
for _ in range(NUM_FLAKES):
    flakes.append(displayio.TileGrid(flake_sheet, pixel_shader=palette,width = 1,height = 1,tile_width = 4,tile_height = 4 ) )

# Snowfield setup
snow_depth = [display.height] * display.width ##Doing this [] * int makes an array
snow_bmp = displayio.Bitmap(display.width, display.height, len(palette)) #Make the snowfield bmp
snow = displayio.TileGrid(snow_bmp, pixel_shader=palette)

# Add everything to display
splash = displayio.Group()
splash.append(background)
splash.append(flakes)
splash.append(snow)
display.show(splash)

def clear_the_snow():
    #pylint: disable=global-statement, redefined-outer-name
    global flakes, flake_pos, snow_depth
    display.auto_refresh = False
    for flake in flakes:
        # set to a random sprite
        flake[0] = randrange(0, 3)
        # set to a random x location
        flake.x = randrange(0, display.width)
    # set random y locations, off screen to start
    #Why they didn't just through this in the loop above is beyond me but hey it works
    flake_pos = [-1.0*randrange(0, display.height) for _ in range(NUM_FLAKES)]
    # reset snow level
    snow_depth = [display.height] * display.width ##again this creates a large array
    # and snow bitmap
    for i in range(display.width * display.height):
        snow_bmp[i] = 0 ##reset the bitmap to zero
    display.auto_refresh = True

##This adds snow to the bitmap down below. If there is too much snow accumulated in one
#region it will actually shift it over and put snow next to it. Pretty cool.
def add_snow(index, amount, steepness=2):
    location = []
    # local steepness check
    for x in range(index - amount, index + amount):
        add = False
        if x == 0:
            # check depth to right
            if snow_depth[x+1] - snow_depth[x] < steepness:
                add = True
        elif x == display.width - 1:
            # check depth to left
            if snow_depth[x-1] - snow_depth[x] < steepness:
                add = True
        elif 0 < x < display.width - 1:
            # check depth to left AND right
            if snow_depth[x-1] - snow_depth[x] < steepness and \
               snow_depth[x+1] - snow_depth[x] < steepness:
                add = True
        if add:
            location.append(x)
    # add where snow is not too steep
    for x in location:
        new_level = snow_depth[x] - 1
        if new_level >= 0:
            snow_depth[x] = new_level
            snow_bmp[x, new_level] = 1

print('Setup Complete')
start_time = time.monotonic()
print(start_time)
##Loop forever
while True:
    ##Go ahead and clear the snow.
    clear_the_snow()
    # loop until globe is full of snow
    while snow_depth.count(0) < display.width:
        # check for shake
        if accelo.shake(SHAKE_THRESHOLD, 5, 0):
            ##wierd way to do this but it basically loops forever unless it fills
            #with snow or you shake the thing
            break
        # update snowflakes
        for i, flake in enumerate(flakes):
            # speed based on sprite index # since the start location
            # was random this is sufficient for speed randomization
            flake_pos[i] += 3 - flake[0] / 3 #The 3 tells the speed so you can make it go faster if you'd like
            # check if snowflake has hit the ground
            if int(flake_pos[i]) >= snow_depth[flake.x]:
                # add snow where it fell
                add_snow(flake.x, flake[0] + 2)
                # reset flake to top
                flake_pos[i] = 0
                # at a new x location
                flake.x = randrange(0, display.width)
            flake.y = int(flake_pos[i]) ##ahh so flake_pos is a float and flake.x and flake.y are ints
            #built into the displayio library. This is kind of like the sprite library I worked on for the Pygamer
        #Then just refresh everything
        display.refresh()
        now_time = time.monotonic()-start_time
        if abs(int(now_time) - now_time) < 0.05:
            print(int(now_time))