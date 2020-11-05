# Metro Matrix Clock
# Runs on Airlift Metro M4 with 64x32 RGB Matrix display & shield
import time
import board
import displayio
import terminalio
import digitalio
from adafruit_display_text.label import Label
from adafruit_bitmap_font import bitmap_font
from adafruit_matrixportal.network import Network
from adafruit_matrixportal.matrix import Matrix

##Button Presses
buttonUP = digitalio.DigitalInOut(board.BUTTON_UP)
buttonUP.direction = digitalio.Direction.INPUT
buttonUP.pull = digitalio.Pull.UP
buttonDOWN = digitalio.DigitalInOut(board.BUTTON_DOWN)
buttonDOWN.direction = digitalio.Direction.INPUT
buttonDOWN.pull = digitalio.Pull.UP

BLINK = True

# Get wifi details and more from a secrets.py file
try:
    from secrets import secrets
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise
print("Matrix Enabled Clock")
print("Time will be set for {}".format(secrets["timezone"]))

# --- Display setup ---
matrix = Matrix()
display = matrix.display
network = Network(status_neopixel=board.NEOPIXEL, debug=False)

# --- Drawing setup ---
group = displayio.Group(max_size=4)  # Create a Group
bitmap = displayio.Bitmap(64, 32, 2)  # Create a bitmap object,width, height, bit depth
color = displayio.Palette(2)  # Create a color palette

###COLORS FOR BACKGROUND AND NUMBERS
                  #white    #red #amber   #blue    #green
colorwheel = [0xFFFFFF,0xFF0000,0xCC4000,0x0000FF,0x85FF00]
backcolorwheel = [0x000000,0xFFFFFF,0xFF0000,0xCC4000,0x0000FF,0x85FF00]
color[0] = backcolorwheel[0] #background - [0] is always the default for the background
color[1] = colorwheel[1]
# Create a TileGrid using the Bitmap and Palette
tile_grid = displayio.TileGrid(bitmap, pixel_shader=color)
group.append(tile_grid)  # Add the TileGrid to the Group
display.show(group)

####CHOOSE FONT HERE
#font = bitmap_font.load_font("/IBMPlexMono-Medium-24_jep.bdf")
font = terminalio.FONT

##Create the Label
clock_label = Label(font, max_glyphs=20)

def update_time(*, hours=None, minutes=None, show_colon=False):
    global temp
    now = time.localtime()  # Get the time values we need

    #Extract Relevant Information
    year = now[0]
    month = now[1]
    day = now[2]
    hours = now[3]
    minutes = now[4]
    seconds = now[5]

    ##Determine if we are AM or PM
    if hours > 12:
        hours-=12
        ampm = 'PM'
    else:
        ampm = 'AM'
        if hours == 0: ##This is midnight
            hours = 12

    ###Determine whether or not to blink the code
    if BLINK:
        colon = ":" if show_colon or now[5] % 2 else " "
    else:
        colon = ":"

    hours_str = str(hours)
    if len(hours_str) == 1:
        hours_str='0'+hours_str
    minutes_str = str(minutes)
    if len(minutes_str) == 1:
        minutes_str='0'+minutes_str
    seconds_str = str(seconds)
    if len(seconds_str) == 1:
        seconds_str='0'+seconds_str
    month_str = str(month)
    if len(month_str) == 1:
        month_str='0'+month_str
    day_str = str(day)
    if len(day_str) == 1:
        day_str='0'+day_str

    ###Print to home for debugging
    print('{}{}{}{}{}{} {}/{}/{} {}F'.format(hours_str,colon,minutes_str,colon,seconds_str,ampm,month_str,day_str,year,temp))

    ###Now print to the RGB Matrix
    ##This prints the year
    #clock_label.text = "{}{}{}{}{}\n{}/{}/{}".format(hours_str,colon,minutes_str,colon,seconds_str,month_str,day_str,year)
    #This prints the temperature
    clock_label.text = "{}{}{}{}{}\n{}/{} {}F".format(hours_str,colon,minutes_str,colon,seconds_str,month_str,day_str,temp)

    ###Get the bounding box
    bbx, bby, bbwidth, bbh = clock_label.bounding_box

    # Then center the label
    clock_label.x = round(display.width / 2 - bbwidth / 2)
    clock_label.y = round(display.height / 2 - bbh*0.35)

###STuff for weather
LOCATION = 'Mobile, US'
UNITS = 'imperial'
DATA_SOURCE = ("http://api.openweathermap.org/data/2.5/weather?q=" + LOCATION + "&units=" + UNITS)
DATA_SOURCE += "&appid=" + secrets["openweather_token"]
DATA_LOCATION = []
temp = 0

last_check = None
update_time(show_colon=True)  # Display whatever time is on the board
group.append(clock_label)  # add the clock label to the group

##The tutorial says to use 1 hour or 60 minutes (3600 seconds) but I think every 10 minutes
##Is more appropriate
RESETINTERVAL = 10*60 #How often do you want the system to check for the time on the internet?
COLORTEXT = 1
COLORBACKGROUND = 0
while True:
    if buttonUP.value == False:
        print('Button Up Pressed')
        COLORTEXT+=1
        if COLORTEXT > 4:
            COLORTEXT = 0
        time.sleep(1)
    if buttonDOWN.value == False:
        print('Button Down Pressed')
        COLORTEXT-=1
        if COLORTEXT < 0:
            COLORTEXT = 0
        #COLORBACKGROUND+=1
        #if COLORBACKGROUND > 5:
        #    COLORBACKGROUND = 0
        time.sleep(1)

    ###SET THE COLOR OF THE CLOCK
    #print(COLORTEXT,COLORBACKGROUND)
    color[0] = backcolorwheel[COLORBACKGROUND]
    clock_label.color = colorwheel[COLORTEXT] ##These colors are set above

    #print('Last Check = ',last_check,'Time = ',time.monotonic())
    if last_check is None or time.monotonic() > last_check + RESETINTERVAL:
        print('Trying to update clock and weather from Internet')
        try:
            print('Updating Time')
            update_time(show_colon=True)  # Make sure a colon is displayed while updating
            print('Getting Time from Network')
            network.get_local_time()  # Synchronize Board's clock to Internet
            print('Getting Weather')
            value = network.fetch_data(DATA_SOURCE, json_path=(DATA_LOCATION,))
            temp = round(value['main']['temp'])
            print(value)
            print('Temperature = ',temp)
            print('Resetting Last Check')
            last_check = time.monotonic()
        except RuntimeError as e:
            print("Some error occured, retrying! -", e)

    update_time()
    time.sleep(1)