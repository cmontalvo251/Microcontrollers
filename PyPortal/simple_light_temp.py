import time
import board
import busio
from digitalio import DigitalInOut
from analogio import AnalogIn

# Import NeoPixel Library
import neopixel
status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2)

# Set up Temperature sensor (ADT7410 sensor)
print("Getting hot")
import adafruit_adt7410
i2c_bus = busio.I2C(board.SCL, board.SDA)
adt = adafruit_adt7410.ADT7410(i2c_bus, address=0x48)
adt.high_resolution = True

# Set up an analog light sensor on the PyPortal
print("Let there be light")
adc = AnalogIn(board.LIGHT)

while True:
    print("===========READING ONBOARD DATA================")
    light_value = adc.value
    print('Light Level: ', light_value)
    temperature_celsius = adt.temperature
    if temperature_celsius > 1.0:
        temperature_farenheit = temperature_celsius*9.0/5.0 + 32.0 - (15.0) #Subsract bias (you need to wait for it to heat up tho)
        print('Temperature: %0.2f F'%(temperature_farenheit))
        print('Current time from start = ',time.monotonic())
    time.sleep(1)