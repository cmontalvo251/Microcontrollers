#IMPORT MODULES
#For Setup
import time
import array
import board
import digitalio
import math
from audiocore import RawSample
from audioio import AudioOut


##Button Presses
a = digitalio.DigitalInOut(board.D4)
a.direction = digitalio.Direction.INPUT
a.pull = digitalio.Pull.DOWN

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
    time.sleep(0.1)  # for the duration of the sleep (in seconds)
    audio.stop()  # and then stop.

##SETUP THE ACCELEROMETER
import busio
import adafruit_lis3dh
##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_16_G
##And the external presure sensor
i2c = busio.I2C(board.SCL, board.SDA)
import adafruit_lps2x
pressure_sensor = adafruit_lps2x.LPS22(i2c)

##SETUP SWITCH
switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP
print('Wiring for Pressure sensor')
print('LPS -> CPB')
print('VIN -> 3.3V')
print('GND -> GND')
print('SCK -> SCL')
print('SDI -> SDA')

###CHECK FOR OPENING FILE
if switch.value == False:
    file = open('Launch_Data_File.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
#thermistor = AnalogIn(board.A9) ##If you want analog
import adafruit_thermistor
thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

###SETUP LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

#For an Indicator
import neopixel
#SETUP PIXELS
pixel_brightness = 0.25
pixels = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=pixel_brightness)
##MAKE A COLOR WHEEL
colors = [(255,0,0),(0,255,0),(0,0,255)]
c = 0

#INFINITE WHILE LOOP
while True:
    pixels[0] = (0,0,0)
    led.value = not led.value
    t = time.monotonic()
    x,y,z = lis3dh.acceleration
    p = pressure_sensor.pressure
    T = thermistor.temperature
    Te = pressure_sensor.temperature
    print(t,x,y,z,p,Te,T)
    play_sound()

    ##CHECK AND SEE IF SWITCH IS THROWN
    if switch.value == False:
        ##CHANGE COLOR EVERY TIME YOU TAKE DATA
        pixels[0] = colors[c]
        c+=1
        if c >= len(colors):
            c = 0
        print('Logging Data')
        ##CHANGE COLOR EVERY TIME YOU TAKE DATA
        output = str(t) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(p) + " " + str(Te) + " " + str(T) + str('\n')
        file.write(output)
        file.flush()
    else:
        print('File is not open. Flip the switch. Then hit reset to start logging')
        if FILEOPEN == True:
            print('FILE CLOSED!!! Hit reset then copy data')
            file.close()
            FILEOPEN = False
        time.sleep(0.1)
