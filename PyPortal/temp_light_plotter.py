import board
import displayio
import time
import busio
import adafruit_adt7410
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn
import terminalio
from adafruit_display_text import label
import adafruit_touchscreen

display = board.DISPLAY

# Create a bitmap with three colors
bitmap = displayio.Bitmap(display.width, display.height, 4)
time_sleep = 0
time_next = 0
time_range_hrs = 2
def clear():
    for x in range(0,display.width):
        for y in range(0,display.height):
            bitmap[x,y] = 0

def createText(level,maxval,minval,x,incolor):
    val = int(level*(maxval-minval)+minval)
    textobj = label.Label(terminalio.FONT,text=str(val),color=incolor)
    textobj.x = int(x*display.width)
    textobj.y = int((1-level)*display.height)
    return textobj

def change_time(time_range_hrs):
    global time_sleep
    #time_range_hrs = 8.0
    #time_next = 0
    time_sleep = time_range_hrs*60*60/display.width
    time_range_text = label.Label(terminalio.FONT,text="R = " + str(time_range_hrs)+" h "+"S = "+str(int(time_sleep))+" s",color=WHITE)
    time_range_text.x = int(0.1*display.width)
    time_range_text.y = int(0.1*display.height)
    print("Sleep Range = ",time_sleep)
    return time_range_text

BLACK = 0x000000
WHITE = 0xffffff #Light color
TEMPCOLOR = 0xFFA500
LIGHTCOLOR = 0xFFFF00

# Create a two color palette
palette = displayio.Palette(4)
palette[0] = BLACK
palette[1] = LIGHTCOLOR
palette[2] = TEMPCOLOR
palette[3] = WHITE

# Create a TileGrid using the Bitmap and Palette
tile_grid = displayio.TileGrid(bitmap, pixel_shader=palette)

# Create a Group
group = displayio.Group(max_size=15)

# Add the TileGrid to the Group
group.append(tile_grid)

#Light step
lightmax = 65536.
lightstep = lightmax/display.height
#Summer
tempmax = 105.
tempmin = 50.
#Winter
#tempmax = 75.
#tempmin = 25.
tempslope = display.height/(tempmin-tempmax)

#Text
tempx = 0.8
temp_texts = []
for i in range(1,10,4):
    print(i/10.)
    temp_texts.append(createText(i/10.,tempmax,tempmin,tempx,TEMPCOLOR))
    group.append(temp_texts[-1])

lightx = 0.7
light_texts = []
for i in range(1,10,4):
    print(i/10.)
    light_texts.append(createText(i/10.,lightmax,0,lightx,LIGHTCOLOR))
    group.append(light_texts[-1])

###Make a grid now
time_range_text = change_time(time_range_hrs)
group.append(time_range_text)

#Setup light and temperature
i2c_bus = busio.I2C(board.SCL, board.SDA)
adt = adafruit_adt7410.ADT7410(i2c_bus, address=0x48)
adt.high_resolution = True
adc = AnalogIn(board.LIGHT)

##Current Temperature and light value
current_vals = label.Label(terminalio.FONT,text="L: "+str(adc.value)+" T: "+str(adt.temperature),color=WHITE)
current_vals.x = int(0.1*display.width)
current_vals.y = int(0.2*display.height)
# Add the Group to the Display
group.append(current_vals)

##THen finally show the group
display.show(group)

#Line Height
line_height = 5

#Create a button for the touch screen
ts = adafruit_touchscreen.Touchscreen(board.TOUCH_XL, board.TOUCH_XR,board.TOUCH_YD, board.TOUCH_YU)

ctr = display.width-1
yball = 0
time_start = time.monotonic()
while 1:
    p = ts.touch_point
    if p:
        print('Screen touched')
        time_range_hrs += 2
        if time_range_hrs > 24:
            time_range_hrs = 2
        current_vals = group[-1]
        #pop the current vals
        group.pop()
        #pop the current labels
        group.pop()
        #Create a new label
        time_range_text = change_time(time_range_hrs)
        #Append the label
        group.append(time_range_text)
        #Append the current_vals
        group.append(current_vals)
        time.sleep(1)
    if time_next < (time.monotonic()-time_start):
        print(time_next,time.monotonic()-time_start)
        time_next += time_sleep
        #Get temperature and light
        light = adc.value
        temperature_celsius = adt.temperature
        temperature_farenheit = temperature_celsius*9.0/5.0 + 32.0 - (15.0) #Subsract bias (you need to wait for it to heat up tho)
        ypixel_temp = int((temperature_farenheit-tempmin)*tempslope+display.height)
        ypixel_light = int(light/lightstep)
        ##Flip the axes
        ypixel_light = display.height - ypixel_light
        print(temperature_farenheit,light,lightstep,tempslope,ypixel_temp,ypixel_light)
        #Update the text
        group.pop()
        current_vals = label.Label(terminalio.FONT,text="L: "+str(light)+" T: "+str(int(temperature_farenheit)),color=WHITE)
        current_vals.x = int(0.1*display.width)
        current_vals.y = int(0.2*display.height)
        group.append(current_vals)
        #Clear bitmap
        #Draw a pixel to indicate what column you are on
        bitmap[ctr,0] = 0
        bitmap[ctr,1] = 0
        bitmap[ctr,2] = 0
        ctr += 1
        if ctr >= display.width:
            ctr = 0
            #Clear the screen
            #clear()
        for x in range(0,display.width):
            for y in range(0,display.height):
                if x == ctr and abs(y-ypixel_temp) < line_height:
                    bitmap[x,y] = 2
                elif x == ctr and abs(y-ypixel_light) < line_height:
                    bitmap[x,y] = 1
                elif x == ctr:
                    bitmap[x,y] = 0
        bitmap[ctr,0] = 3
        bitmap[ctr,1] = 3
        bitmap[ctr,2] = 3
    ##Draw a ball dropping on the right side of the screen
    xball = int(0.9*display.width)
    bitmap[xball,yball] = 0
    yball += 1
    if yball >= display.height:
        yball = 0
    bitmap[xball,yball] = 3
    #Time sleep
    #print(time_next,time.monotonic()-time_start)
    time.sleep(0.01)