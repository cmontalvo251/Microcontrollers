#IMPORT MODULES
#For Setup
import time
import busio
import board
import digitalio

#For Sensors Used
import adafruit_lis3dh
from adafruit_ms8607 import MS8607
import adafruit_thermistor

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

#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
#thermistor = AnalogIn(board.A9) ##If you want analog
thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

##SETUP THE ACCELEROMETER
##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

##And the external accel/gyro/mag
##Finally PTH Sensor
i2c = busio.I2C(board.SCL, board.SDA)
pth = MS8607(i2c)

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

##MAKE A COLOR WHEEL
colors = [(255,0,0),(0,255,0),(0,0,255)]
c = 0

###CHECK FOR OPENING FILE
if switch.value == False:
    file = open('CPB_Datalog.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

###BOOLEANS
ADVERTISING = False

#INFINITE WHILE LOOP
while True:
    ##RESET PIXELS
    pixels[0] = (0,0,0)
    led.value = not led.value

    ##GET CURRENT TIME
    t = time.monotonic()

    ##GET THE CURRENT ACCEL
    x,y,z = lis3dh.acceleration

    #and PTH sensor
    p = pth.pressure
    Te = pth.temperature
    rH = pth.relative_humidity

    #GET THE TEMPERATURE
    T = thermistor.temperature

    ##PRINT TO STDOUT
    print((t,x,y,z,p,rH,Te,T))

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
        uart_server.write('{},{},{},{},{}\n'.format(t,x,y,z,T))

    ##CHECK AND SEE IF SWITCH IS THROWN
    if switch.value == False:
        ##CHANGE COLOR EVERY TIME YOU TAKE DATA
        pixels[0] = colors[c]
        c+=1
        if c >= len(colors):
            c = 0
        #PRINT TO A FILE
        #print((t,x,y,z,p,rH,Te,T))
        output = str(t) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(p) + " " + str(rH) + " " + str(Te) + " " + str(T) + str('\n')
        file.write(output)
        file.flush()
    else:
        if FILEOPEN == True:
            print('FILE CLOSED!!!')
            file.close()
            FILEOPEN = False

    ##Wait 0.2 seconds for 5 hz data rate
    time.sleep(0.2)# Write your code here :-)
