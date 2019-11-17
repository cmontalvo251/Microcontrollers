import audioio
import time
import board
import neopixel
from digitalio import DigitalInOut, Direction, Pull
import digitalio
import busio
import digitalio
import adafruit_lis3dh
import touchio
import random
import pulseio

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
a = audioio.AudioOut(board.A0)

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, auto_write=True)
flora = neopixel.NeoPixel(board.A2,2,auto_write=True)

#and sequins
pwm_leds1 = board.D0 ##Pin D0 is pin A6. It can be digital or analog
pwm1 = pulseio.PWMOut(pwm_leds1, frequency=1000, duty_cycle=0) 
pwm_leds2 = board.D10 ##Pin D10 is pin A3. It can be digital or analog
pwm2 = pulseio.PWMOut(pwm_leds2, frequency=1000, duty_cycle=0) 

##Button Presses
buttonA = DigitalInOut(board.BUTTON_A)
buttonA.direction = Direction.INPUT
buttonA.pull = Pull.DOWN
buttonB = DigitalInOut(board.BUTTON_B)
buttonB.direction = Direction.INPUT
buttonB.pull = Pull.DOWN

#Slide Switch
slide = DigitalInOut(board.SLIDE_SWITCH)
#print(dir(board))

#Set up Touch Buttons
touch1 = touchio.TouchIn(board.A1)

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

def check_brightness():
    global brightness
    max_brightness = 40
    if brightness > max_brightness:
        brightness = 0
    if brightness < 0:
        brightness = max_brightness
    print("Current Brightness = ",brightness)
        
def change_brightness(increment):
    print("Changing Brightness by = ",increment)
    global brightness
    brightness+=increment
    check_brightness()

def Get_Sound():
    print("Importing Sound")
    f = open("Feliz.wav", "rb")
    wav = audioio.WaveFile(f)
    return f,wav
    
def CHANGE_MODE():
    global MODE
    MODE+=1
    if MODE > 4:
        MODE = 0
    print("Current MODE = ",MODE)
    
def get_color(now_time):
    global MODE,XMAS,change_time
    color = (brightness,brightness,brightness)
    if MODE == 0:
        color = (brightness,0,0)
    if MODE == 1:
        color = (0,brightness,0)
    if MODE == 2:
        color = (0,0,brightness)
    if MODE == 3:
        r = random.randint(0,255)
        g = random.randint(0,255)
        b = random.randint(0,255)
        norm = (r*r + g*g + b*b)**(0.5)
        #print(r/norm,g/norm,b/norm,norm)
        rscale = int(r/norm*brightness)
        gscale = int(g/norm*brightness)
        bscale = int(b/norm*brightness)
        color = (rscale,gscale,bscale)
    if MODE == 4:
        if now_time - change_time > 2:
            XMAS = not XMAS
            change_time = now_time
        if XMAS:
            color_main = (brightness,0,0)
            color_alt = (0,brightness,0)
        else:
            color_main = (0,brightness,0)
            color_alt = (brightness,0,0)
    else:
        color_main = color
        color_alt = color
    return color_main,color_alt
    
##Take a bunch of average readings
avg = 0
N = 100
for i in range(0,N):
    x,y,z = lis3dh.acceleration
    norm2 = x*x + y*y + z*z
    avg+=norm2
avg/=(N+1)

#Initialize Sound
f,wav = Get_Sound()

#Globals
MODE = 4
sparkle_start = 0
change_time = 0
SPARKLE = False
XMAS = True
brightness = 10
color = get_color(0)
FLASH = True
    
while True:
    #Get Current Time for Event Handling
    now_time = time.monotonic()
    #print("Time = ",now_time)
    
    #Button A event
    if buttonA.value:
        CHANGE_MODE()
        time.sleep(0.25)
        
    #Button B event
    if a.playing:
        pass
    else:
        if buttonB.value:
            f.close()
            f,wav = Get_Sound()
            print("Playing Sound")
            a.play(wav)
        
    #Slide Switch Event
    if slide.value:
        SEQUINS = True #This is only for perpetual sequins
    else:
        SEQUINS = False
    
    #Capacitive Touch Event
    if touch1.value:
        change_brightness(1)
 
    #Accelerometer Event
    x,y,z = lis3dh.acceleration
    norm2 = x*x + y*y + z*z
    #print('Accel Diff = ',norm2-avg)
    if abs(norm2 - avg) > 2000:
        print("Sparkling")
        SPARKLE = True
        sparkle_start = now_time
    if now_time - sparkle_start > 1 and SPARKLE:
        print("Sparkles off")
        SPARKLE = False
        
    ##Now we actually do stuff based on the following settings
    #brightness
    #SPARKLE -- tells us if we need to be sparkling
    #SEQUINS -- tells us if sequins are always on
    #MODE -- which mode we are in (solid red,blue, green, random, red/green duty cycle)
    
    #Alright let's do pixels on the CPX first
    color_main,color_alt = get_color(now_time)
    pixels.fill(color_main)
    
    #Now we need to turn on the flora's
    flora[0] = color_alt
    flora[1] = color_main
    
    #Then we need run the sequins
    if SEQUINS or SPARKLE:
        if not FLASH:
            pwm1.duty_cycle = int(brightness*65535./(6*255.))
            pwm2.duty_cycle = int(brightness*65535./(255.))
            if now_time - sparkle_time > 0.2:
                FLASH = True
        if FLASH:
            FLASH = False
            sparkle_time = now_time
            pwm1.duty_cycle = int(brightness*65535./255.)
            pwm2.duty_cycle = int(brightness*65535./(6*255.))
    else:
        pwm1.duty_cycle = 0
        pwm2.duty_cycle = 0
        
    #Slow Down Sim
    time.sleep(0.1)