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
IO_DELAY = 30
 
# Get wifi details and more from a secrets.py file
try:
    from secrets import secrets
    print("Secrets Imported")
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise

import supervisor
 
# PyPortal ESP32 Setup
try:
    esp32_cs = DigitalInOut(board.ESP_CS)
    esp32_ready = DigitalInOut(board.ESP_BUSY)
    esp32_reset = DigitalInOut(board.ESP_RESET)
    spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
    esp = adafruit_esp32spi.ESP_SPIcontrol(spi, esp32_cs, esp32_ready, esp32_reset)
    #Oh this is just a status light
    status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2) 
    #Set up wifi
    wifi = adafruit_esp32spi_wifimanager.ESPSPI_WiFiManager(esp, secrets, status_light)
except e:
    print(e)
    supervisor.reload()
# Set your Adafruit IO Username and Key in secrets.py
# (visit io.adafruit.com if you need to create an account,
# or if you need your Adafruit IO key.)
ADAFRUIT_IO_USER = secrets['adafruit_io_user'] #These are just dictionary calls
ADAFRUIT_IO_KEY = secrets['adafruit_io_key']
 
# Create an instance of the Adafruit IO REST client
io = RESTClient(ADAFRUIT_IO_USER, ADAFRUIT_IO_KEY, wifi) #Connect to the internet

print("Connected to the Internet")

#This probably isn't needed but whatever - this is needed but I'm getting an error. The error was with Adafruit IO. You can't have _ symbols
try:
    # Get the 'temperature' feed from Adafruit IO
    print("Handshake Attempt")
    temperature_feed = io.get_feed('temperature-solar-oven')
    print('Got temperature Feed')
    light_feed = io.get_feed('light-solar-oven')
    print('Got light Feed')
    battery_feed = io.get_feed('battery-voltage')
    print('Got battery feed')
    solar_feed = io.get_feed('solar-voltage')
    print('Got solar feed')
    print("Received Adafruit IO Handshake")
except AdafruitIO_RequestError:
    # If no 'temperature' feed exists, create one
    print("Could not find feed")
    temperature_feed = io.create_new_feed('temperature-solar-oven')
    print("New Feed Created - Temperature")
    light_feed = io.create_new_feed('light-solar-oven')
    print("New Feed Created - Light")
    battery_feed = io.create_new_feed('battery-voltage')
    print("New Feed Created - Battery Voltage")
    solar_feed = io.create_new_feed('solar-voltage')
 
# Set up Temperature sensor (ADT7410 sensor)
print("Getting hot")
i2c_bus = busio.I2C(board.SCL, board.SDA)
adt = adafruit_adt7410.ADT7410(i2c_bus, address=0x48)
adt.high_resolution = True
 
# Set up an analog light sensor on the PyPortal
print("Let there be light")
light = AnalogIn(board.LIGHT)
solar = AnalogIn(board.D3)
battery = AnalogIn(board.D4)
#ctr = 1

numData = 0
while numData < 2:
    numData+=1
    try:
        print("===========READING ONBOARD DATA================")
        print("Number of Data Reads since last reboot = ",numData)
        light_value = light.value
        print('Light Level: ', light_value)
        solar_value = solar.value*5.0/65536.
        print('Solar Voltage = ',solar_value)
        battery_value = battery.value*5.0/65536.
        print('Battery Voltage = ',battery_value)
        temperature_celsius = adt.temperature
        if temperature_celsius > 1.0:
            temperature_farenheit = temperature_celsius*9.0/5.0 + 32.0
            print('Temperature: %0.2f F'%(temperature_farenheit))

            print("===========SENDING TO LADY ADA=================")
            io.send_data(light_feed['key'], light_value)
            print('Light Value sent')
            io.send_data(temperature_feed['key'], temperature_farenheit, precision=2)
            print('Temp Data Sent')
            io.send_data(battery_feed['key'],battery_value,precision=3)
            print('Battery Voltage Sent')
            io.send_data(solar_feed['key'],solar_value,precision=3)
            print('Solar Voltage Sent')
            print('=================DATA SENT======================')
        else:
            print("Temperature error. Skipping")
    except e:
        print("ERROR!!!", e)
        #wifi.reset()
        supervisor.reload()
        #continue
    print('==============Delaying {0} seconds================='.format(IO_DELAY))
    #print('TURNING OFF SCREEN AFTER DELAY')
    time.sleep(IO_DELAY)
supervisor.reload()