# Metro Matrix Clock
# Runs on Airlift Metro M4 with 64x32 RGB Matrix display & shield
import time
import board
import displayio
import terminalio
import digitalio
import sys
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

#Create Timer
network_timer = 0.0

##Update Settings
update_network = 3600.0

##Network Update Function
def Network_Update(temp_in):
    print('Trying to update clock and weather from Internet')
    try:
        print('Updating Time')
        network.get_local_time()  # Synchronize Board's clock to Internet
        print("Fetching Json from",DATA_SOURCE)
        value = network.fetch_data(DATA_SOURCE, json_path=(DATA_LOCATION,))
        temp_out = round(value['main']['temp'])
        print('Received Temperature = ',temp_out)
        network_timer = current_time + update_network
    except:
        print("Could not update values from internet.")
        print("Using old value for temp")
        temp_out = temp_in
        print("Going to try again in 1 minute")
        network_timer = current_time + 60.0
    return network_timer,temp_out

def update_text(*,temp_in=0,hours=None, minutes=None, show_colon=False):
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
    print('{}{}{}{}{}{} {}/{}/{} {}F'.format(hours_str,colon,minutes_str,colon,seconds_str,ampm,month_str,day_str,year,temp_in))

    ###Now print to the RGB Matrix
    ##This prints the year
    #clock_label.text = "{}{}{}{}{}\n{}/{}/{}".format(hours_str,colon,minutes_str,colon,seconds_str,month_str,day_str,year)
    #This prints the temperature
    clock_label.text = "{}{}{}{}{}\n{}/{} {}F".format(hours_str,colon,minutes_str,colon,seconds_str,month_str,day_str,temp_in)

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
temp = 0.0

update_text(show_colon=True)  # Display whatever time is on the board
group.append(clock_label)  # add the clock label to the group

##The tutorial says to use 1 hour or 60 minutes (3600 seconds) but I think every 10 minutes
##Is more appropriate
minutes2seconds = 60.0
COLORTEXT = 1
COLORBACKGROUND = 0

while True:
    #################THIS IS WHERE WE CHECK FOR BUTTON PRESSES#############
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
    #######################################################################

    ######################SET THE COLOR OF THE CLOCK####################
    #print(COLORTEXT,COLORBACKGROUND)
    color[0] = backcolorwheel[COLORBACKGROUND]
    clock_label.color = colorwheel[COLORTEXT] ##These colors are set above
    ########################################################################

    ########################NOTIFY USER OF PROGRESS########################
    current_time = time.monotonic()
    print("Current Time = ",current_time," Next network update = ",network_timer)

    ##########################CHECK FOR NETWORK UPDATE##########################
    if current_time - network_timer > update_network or network_timer == 0:
        network_timer,temp = Network_Update(temp)

    ###########################UPDATE CLOCK TEXT######################
    update_text(temp_in=temp)

    ######################SLEEP##################################3
    time.sleep(0.1)
