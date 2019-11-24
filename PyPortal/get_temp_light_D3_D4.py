import time
import board
import busio
from digitalio import DigitalInOut
from analogio import AnalogIn
# ESP32 SPI
from adafruit_esp32spi import adafruit_esp32spi, adafruit_esp32spi_wifimanager 
# Import NeoPixel Library
import neopixel
# Import Adafruit IO REST Client
from adafruit_io.adafruit_io import RESTClient, AdafruitIO_RequestError
# Import ADT7410 Library
import adafruit_adt7410
 
# Timeout between sending data to Adafruit IO, in seconds
IO_DELAY = 0.1
#Oh this is just a status light
status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2) 
# Set up Temperature sensor (ADT7410 sensor)

print("Getting hot")
i2c_bus = busio.I2C(board.SCL, board.SDA)
adt = adafruit_adt7410.ADT7410(i2c_bus, address=0x48)
adt.high_resolution = True
 
# Set up an analog light sensor on the PyPortal
print("Let there be light")
light = AnalogIn(board.LIGHT)
#ctr = 1

solar = AnalogIn(board.D3)
battery = AnalogIn(board.D4)

while True:
    #Take Readings
    light_value = light.value
    temperature_celsius = adt.temperature
    temperature_farenheit = temperature_celsius*9.0/5.0 + 32.0
    solar_voltage = solar.value*5.0/65536.
    battery_voltage = battery.value*5.0/65536.
    print('L: ', light_value," T: ",temperature_farenheit,"B&S(V): ",battery_voltage,solar_voltage)
    time.sleep(IO_DELAY)