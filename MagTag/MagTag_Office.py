import time
import random
import neopixel
import digitalio
import terminalio
import ssl
import wifi
import socketpool
import adafruit_requests
#import audioio
import board
from adafruit_magtag.magtag import MagTag

# enable the speaker
#speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
#speaker_enable.direction = digitalio.Direction.OUTPUT
#speaker_enable.value = True
#a = audioio.AudioOut(board.A0)

###External light strip
NUM_PIXELS = 30
light_strip = neopixel.NeoPixel(board.D10, NUM_PIXELS, auto_write=True)

##Main MagTag object
magtag = MagTag()

# lights up fast
magtag.peripherals.neopixels.brightness = 0.1
magtag.peripherals.neopixel_disable = False # turn on lights
light_strip.fill(0xFF0000)

# Get wifi details and more from a secrets.py file
try:
    from secrets import secrets
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise
print("Time will be set for {}".format(secrets["timezone"]))

#Connect to Wifi
print("Connecting to Wifi")
wifi.radio.connect(secrets["ssid"], secrets["password"])

##Get Pool
print('Getting Pool')
pool = socketpool.SocketPool(wifi.radio)

##Get Requests
print('Setting up requests')
requests = adafruit_requests.Session(pool, ssl.create_default_context())

#Weather Stuff
LOCATION = 'Mobile, US'
UNITS = 'imperial'
DATA_SOURCE = ("http://api.openweathermap.org/data/2.5/weather?q=" + LOCATION + "&units=" + UNITS)
DATA_SOURCE += "&appid=" + secrets["openweather_token"]
temp = 0.0

#Test Weather
#print("Fetching json from", DATA_SOURCE)
#response = requests.get(DATA_SOURCE)
#temp = round(response['main']['temp'])
#print(response.json())
#print(response.json()["main"]["temp"])
#temp = response.json()["main"]["temp"]
#print("Received Temperature = ",temp)

# main text, index 0
magtag.add_text(
    #text_font = "Arial-18.bdf",
    text_font = terminalio.FONT,
    text_position=(
        magtag.graphics.display.width // 2,
        10,
    ),
    text_scale = 4.0,
    line_spacing=1,
    text_anchor_point=(0.5, 0),
)


def Get_Sound():
    print("Importing Sound")
    f = open("poke_center.wav", "rb")
    wav = audioio.WaveFile(f)
    return f,wav

def play_fun_song():
    f.close()
    f,wav = Get_Sound()
    print("Playing Sound")
    a.play(wav)

def PLAY_DEMO_SONG():
    song = ((330, 1), (370, 1), (392, 2), (370, 2), (330, 2), (330, 1),
           (370, 1), (392, 1), (494, 1), (370, 1), (392, 1), (330, 2))
    for notepair in song:
        magtag.peripherals.play_tone(notepair[0], notepair[1] * 0.2)

def get_random_color():
    r = random.randint(0,255)
    g = random.randint(0,255)
    b = random.randint(0,255)
    norm = (r*r + g*g + b*b)**(0.5)
    #print(r/norm,g/norm,b/norm,norm)
    brightness = 255
    rscale = int(r/norm*brightness)
    gscale = int(g/norm*brightness)
    bscale = int(b/norm*brightness)
    color = (rscale,gscale,bscale)
    return color

def update_text():
    global temp

    #Get Temperature
    print("Fetching json from", DATA_SOURCE)
    response = requests.get(DATA_SOURCE)
    print(response.json())
    print(response.json()["main"]["temp"])
    temp = round(response.json()["main"]["temp"])
    print("Received Temperature = ",temp)

    ##Get internet time
    magtag.network.get_local_time()
    now = time.localtime()  # Get the time values we need

    #Extract Relevant Information
    year = now[0]
    month = now[1]
    day = now[2]
    hours = now[3]
    minutes = now[4]

    ##Determine if we are AM or PM
    if hours > 12:
        hours-=12
        ampm = 'PM'
    else:
        ampm = 'AM'
        if hours == 0: ##This is midnight
            hours = 12

    ###Determine whether or not to blink the code
    colon = ":"

    hours_str = str(hours)
    if len(hours_str) == 1:
        hours_str='0'+hours_str
    minutes_str = str(minutes)
    if len(minutes_str) == 1:
        minutes_str='0'+minutes_str
    month_str = str(month)
    if len(month_str) == 1:
        month_str='0'+month_str
    day_str = str(day)
    if len(day_str) == 1:
        day_str='0'+day_str

    ###Print to home for debugging
    print('{}{}{}{} {}/{}/{} {}F'.format(hours_str,colon,minutes_str,ampm,month_str,day_str,year,temp))

    ###Now print to the screen
    text = "{}{}{} {}\n{}/{} {}F".format(hours_str,colon,minutes_str,ampm,month_str,day_str,temp)
    magtag.set_text(text)

#Set background to white
magtag.set_background(0xFFFFFF)

##Create a Timer
pixel_timer = 0.0
screen_timer = 0.0

#Update settings
update_pixels = 1.0
update_minutes = 1.0
update_screen = update_minutes*60.0 ##minutes to seconds

while True:
    #Get current time
    current_time = time.monotonic()

    ##Get current button presses
    for i, b in enumerate(magtag.peripherals.buttons):
        if not b.value:
            if i == 0:
                print("Button 0 Pressed")
                PLAY_DEMO_SONG()
                #play_fun_song()

    ##Check to see if it's time to update the Pixels
    if current_time - pixel_timer > update_pixels:
        print("updating pixels")
        pixel_timer = current_time + update_pixels
        #on board pixels first
        for x in range(0,4):
            magtag.peripherals.neopixels[x] = get_random_color()

        #Then led strip
        for x in range(0,NUM_PIXELS):
            light_strip[x] = get_random_color()

    ##Now check to see if we should update the screen
    if current_time - screen_timer > update_screen or screen_timer == 0:
        print("updating screen")
        screen_timer = current_time + update_screen
        update_text()

    ##Slow things down for a moment
    time.sleep(0.1)