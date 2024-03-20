#IMPORT MODULES
#For Setup
import time
import busio
import board
import digitalio

#Accelerometer
import adafruit_lis3dh

#For Sensors Used
import adafruit_gps

#For an Indicator
import neopixel

###SETUP LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##SETUP SWITCH
switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

#SETUP PIXELS
pixel_brightness = 0.25
pixels = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=pixel_brightness)

#SETUP GPS
uart = busio.UART(board.TX, board.RX, baudrate=9600, timeout=10)
gps = adafruit_gps.GPS(uart, debug=False)  # Use UART/pyserial
gps.send_command(b"PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")
gps.send_command(b"PMTK220,1000")

##SETUP ACCELEROMETER
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G


##MAKE A COLOR WHEEL
colors = [(255,0,0),(0,255,0),(0,0,255)]
c = 0

###CHECK FOR OPENING FILE
if switch.value == False:
    file = open('GPS_Data_File.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

#INFINITE WHILE LOOP
last_print = time.monotonic()
while True:
    ##GET CURRENT TIME
    t = time.monotonic()

    ##GET THE CURRENT GPS
    gps.update()
    latitude = gps.latitude
    longitude = gps.longitude
    altitude = gps.altitude_m

    #GET CURRENT ACCEL VALUES
    x,y,z = lis3dh.acceleration

    ##PRINT TO STDOUT
    if t - last_print >= 0.1:
        ##RESET PIXELS
        pixels[0] = (0,0,0)
        led.value = not led.value
        last_print = t
        print((t,latitude,longitude,altitude,x,y,z))

        ##CHECK AND SEE IF SWITCH IS THROWN
        if switch.value == False:
            ##CHANGE COLOR EVERY TIME YOU TAKE DATA
            pixels[0] = colors[c]
            c+=1
            if c >= len(colors):
                c = 0
            #PRINT TO A FILE
            output = str(t) + " " + str(latitude) + " " + str(longitude) + " " + str(altitude) + " " + str(x) + " " + str(y) + " " + str(z) + str('\n')
            file.write(output)
            file.flush()
        else:
            if FILEOPEN == True:
                print('FILE CLOSED!!!')
                file.close()
                FILEOPEN = False
