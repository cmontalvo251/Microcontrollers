###Import time,board,analogio and adafruit_thermistor for temperature
import time
import board
import analogio
import adafruit_thermistor

##Import Adafruit BLE
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

##Setup BLE
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)

import adafruit_lis3dh
import digitalio
import busio
import math

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

import neopixel
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.2, auto_write=True)
RED = (255, 0, 0)
BLUE = (0,0,255)
GREEN = (0,255,0)
STEPS = 0
CTR = 0
RESET = False
while True:
    #Get accelerometer data
    x,y,z = lis3dh.acceleration
    norm = math.sqrt(x**2 + y**2 + z**2)
    bias = norm - 9.81
    # Advertise when not connected.
    ble.start_advertising(advertisement)
    ##Do the color_chase function when ble is not connected
    while not ble.connected:
        print('Not Connected',time.monotonic(),'Look for ',ble.name)
        pixels.fill(RED)
        time.sleep(.2)
        pixels.fill(BLUE)
        time.sleep(0.2)
    ble.stop_advertising()
    print('CONNECTED!',time.monotonic())
    if bias > 0 and RESET == False:
        STEPS += 1
        CTR+=1
        RESET = True
    if bias < 0 and RESET == True:
        RESET = False
    if CTR > 9:
        CTR = 0
        pixels.fill((0,0,0))
    pixels[CTR] = RED
    uart_server.write("{},{},{},{}\n".format(time.monotonic(),x,y,z))
    time.sleep(0.01)