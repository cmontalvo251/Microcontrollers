#IMPORT MODULES
import time
import busio
import board
import digitalio
import adafruit_lis3dh
import adafruit_thermistor
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

###SETUP LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##SETUP SWITCh
switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

#Set up pixels
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
else:
    print('Not opening file for writing')

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

    #GET THE TEMPERATURE
    T = thermistor.temperature

    ##PRINT TO STDOUT
    print(t,x,y,z,T)

    # Advertise when not connected.
    if not ble.connected:
        print('Not connected')
        if ADVERTISING == False:
            ble.start_advertising(advertisement)
            ADVERTISING = True
        else:
            print('Advertising')
    else:
        #Stop advertising once connected
        print('Connected')
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
        output = str(t) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(T) + str('\n')
        file.write(output)
        file.flush()

    ##Wait 0.2 seconds for 5 hz data rate
    time.sleep(0.2)# Write your code here :-)
