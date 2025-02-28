#IMPORT MODULES
#For Setup
import time
import busio
import board
import digitalio

#For Sensors Used
import adafruit_gps
print('GPS wiring (CPX -> GPS)')
print('A6/RX -> TX')
print('A7/TX -> RX')
print('GND -> GND')
print('VOUT -> VIN')
print('Sleeping 5 seconds...')
time.sleep(5.0)

#Import accelerometer
import adafruit_lis3dh

#For an Indicator
import neopixel

#For Bluetooth
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

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

##SETUP ACCELEROMETER
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G


####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

#SETUP GPS
uart = busio.UART(board.TX, board.RX, baudrate=9600, timeout=10)
gps = adafruit_gps.GPS(uart, debug=False)  # Use UART/pyserial
gps.send_command(b"PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")
gps.send_command(b"PMTK220,1000")

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

###BOOLEANS
ADVERTISING = False

print(dir(gps))

#INFINITE WHILE LOOP
last_print = 0.0
start_time = time.monotonic()
while True:
    ##GET CURRENT TIME
    t = time.monotonic()-start_time

    ##GET THE CURRENT GPS
    gps.update()
    latitude = gps.latitude
    longitude = gps.longitude
    altitude = gps.altitude_m
    speed = gps.speed_knots
    if latitude is None:
        latitude = -99
    if longitude is None:
        longitude = -99
    if altitude is None:
        altitude = -99
    if speed is None:
        speed = -99

    ##GET ACCELEROMTER
    x,y,z = lis3dh.acceleration

    ##PRINT TO STDOUT
    if t - last_print >= 1.0:
        ##RESET PIXELS
        pixels[0] = (0,0,0)
        led.value = not led.value
        last_print = t
        print((t,latitude,longitude,altitude,speed,x,y,z))
        print(gps._raw_sentence)

        # Advertise when not connected.
        if not ble.connected:
            if ADVERTISING == False:
                ble.start_advertising(advertisement)
                ADVERTISING = True
            else:
                print('Advertising, Look for ',ble.name)
        else:
            #Stop advertising once connected
            ble.stop_advertising()
            ADVERTISING = False
            uart_server.write('{},{},{},{},{},{},{},{}\n'.format(t,latitude,longitude,altitude,speed,x,y,z))

        ##CHECK AND SEE IF SWITCH IS THROWN
        if switch.value == False:
            ##CHANGE COLOR EVERY TIME YOU TAKE DATA
            pixels[0] = colors[c]
            c+=1
            if c >= len(colors):
                c = 0
            #PRINT TO A FILE
            output = str(t) + "," + "{0:.6f},{1:.6f}".format(latitude,longitude) + "," + str(altitude) + "," + str(speed) + "," + str(x) + "," + str(y) + "," + str(z) + str('\n')
            file.write(output)
            file.flush()
        else:
            if FILEOPEN == True:
                print('FILE CLOSED!!!')
                file.close()
                FILEOPEN = False
