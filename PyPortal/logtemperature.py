"""
PyPortal IOT Data Logger for Adafruit IO
 
Dependencies:
    * CircuitPython_ADT7410
        https://github.com/adafruit/Adafruit_CircuitPython_ADT7410
 
    * CircuitPython_AdafruitIO
        https://github.com/adafruit/Adafruit_CircuitPython_AdafruitIO
"""
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
IO_DELAY = 10
 
# Get wifi details and more from a secrets.py file
try:
    from secrets import secrets
    print("Secrets Imported")
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise
 
# PyPortal ESP32 Setup
esp32_cs = DigitalInOut(board.ESP_CS)
esp32_ready = DigitalInOut(board.ESP_BUSY)
esp32_reset = DigitalInOut(board.ESP_RESET)
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
esp = adafruit_esp32spi.ESP_SPIcontrol(spi, esp32_cs, esp32_ready, esp32_reset)
#Oh this is just a status light
status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2) 
#Set up wifi
wifi = adafruit_esp32spi_wifimanager.ESPSPI_WiFiManager(esp, secrets, status_light)
 
# Set your Adafruit IO Username and Key in secrets.py
# (visit io.adafruit.com if you need to create an account,
# or if you need your Adafruit IO key.)
ADAFRUIT_IO_USER = secrets['adafruit_io_user'] #These are just dictionary calls
ADAFRUIT_IO_KEY = secrets['adafruit_io_key']
 
# Create an instance of the Adafruit IO REST client
io = RESTClient(ADAFRUIT_IO_USER, ADAFRUIT_IO_KEY, wifi) #Connect to the internet

print("Connected to the Internet")

#This probably isn't needed but whatever - this is needed but I'm getting an error
try:
    # Get the 'temperature' feed from Adafruit IO
    print("Handshake Attempt")
    temperature_feed = io.get_feed('temperature-solar-oven')
    light_feed = io.get_feed('light-solar-oven')
    print("Received Adafruit IO Handshake")
except AdafruitIO_RequestError:
    # If no 'temperature' feed exists, create one
    print("Could not find feed")
    temperature_feed = io.create_new_feed('temperature-solar-oven')
    print("New Feed Created - Temperature")
    light_feed = io.create_new_feed('light-solar-oven')
    print("New Feed Created - Light")
 
# Set up Temperature sensor (ADT7410 sensor)
print("Getting hot")
i2c_bus = busio.I2C(board.SCL, board.SDA)
adt = adafruit_adt7410.ADT7410(i2c_bus, address=0x48)
adt.high_resolution = True
 
# Set up an analog light sensor on the PyPortal
print("Let there be light")
adc = AnalogIn(board.LIGHT)
 
while True:
    try:
        print("===========READING ONBOARD DATA================")
        light_value = adc.value
        print('Light Level: ', light_value)
        temperature_celsius = adt.temperature
        temperature_farenheit = temperature_celsius*9.0/5.0 + 32.0
        print('Temperature: %0.2f F'%(temperature_farenheit))

        print("===========SENDING TO LADY ADA=================")
        io.send_data(light_feed['key'], light_value)
        io.send_data(temperature_feed['key'], temperature_farenheit, precision=2)
        print('=================DATA SENT======================')
    except (ValueError, RuntimeError) as e:
        print("ERROR!!!", e)
        wifi.reset()
        continue
    print('==============Delaying {0} seconds================='.format(IO_DELAY))
    time.sleep(IO_DELAY)
