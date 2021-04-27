# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

# adafruit_circuitpython_adafruitio usage with an esp32spi_socket
from random import randint
import board
import busio
import time
from digitalio import DigitalInOut
import adafruit_esp32spi.adafruit_esp32spi_socket as socket
from adafruit_esp32spi import adafruit_esp32spi, adafruit_esp32spi_wifimanager
import adafruit_requests as requests
from adafruit_io.adafruit_io import IO_HTTP, AdafruitIO_RequestError

from audiocore import RawSample
import array
import digitalio
import neopixel
import math
from audioio import AudioOut

# If you are using a board with pre-defined ESP32 Pins:
esp32_cs = DigitalInOut(board.ESP_CS)
esp32_ready = DigitalInOut(board.ESP_BUSY)
esp32_reset = DigitalInOut(board.ESP_RESET)
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
esp = adafruit_esp32spi.ESP_SPIcontrol(spi, esp32_cs, esp32_ready, esp32_reset)

# Get wifi details and more from a secrets.py file
try:
    from secrets import secrets
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise
    
print('Secrets Imported')

status_light = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.2)
wifi = adafruit_esp32spi_wifimanager.ESPSPI_WiFiManager(esp, secrets, status_light)

print('Connected to Wifi')

# Set your Adafruit IO Username and Key in secrets.py
# (visit io.adafruit.com if you need to create an account,
# or if you need your Adafruit IO key.)
aio_username = secrets['aio_username']
aio_key = secrets['aio_key']
 
# Create an instance of the IO_HTTP client
print('Trying IO')
io = IO_HTTP(aio_username, aio_key, wifi)
print('IO Instance Created')

# Get the weight feed from IO
moisture_feed = io.get_feed('moisture')
temperature_feed = io.get_feed('temperature')
print('Feeds obtained')

FREQUENCY = 440  # 440 Hz middle 'A'
SAMPLERATE = 8000  # 8000 samples/second, recommended!

# Generate one period of sine wav.
length = SAMPLERATE // FREQUENCY
sine_wave = array.array("H", [0] * length)
for i in range(length):
    sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)

# Enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True

####Generates the audio object
audio = AudioOut(board.SPEAKER)
sine_wave_sample = RawSample(sine_wave)

# A single sine wave sample is hundredths of a second long. If you set loop=False, it will play
# a single instance of the sample (a quick burst of sound) and then silence for the rest of the
# duration of the time.sleep(). If loop=True, it will play the single instance of the sample
# continuously for the duration of the time.sleep().
def play_sound():
    audio.play(sine_wave_sample, loop=True)  # Play the single sine_wave sample continuously...
    time.sleep(1)  # for the duration of the sleep (in seconds)
    audio.stop()  # and then stop.


# Send random integer values to the feed
while True:
    moisture = randint(0, 200)
    temperature = randint(0, 95)
    print('Number = ',moisture)
    print('Number = ',temperature)
    if moisture > 150 :
        ###Play Sound
        play_sound( )
        print('Playing a sound')
    if temperature > 90:
        #Play Sound
        play_sound()
        print('Playing a sound')
    print("Sending {0} to moisture feed...".format(moisture))
    io.send_data(moisture_feed["key"], moisture)
    print("Sending {0} to temperature feed...".format(temperature))
    io.send_data(temperature["key"], temperature)
    print("Data sent!")
    time.sleep(1)
