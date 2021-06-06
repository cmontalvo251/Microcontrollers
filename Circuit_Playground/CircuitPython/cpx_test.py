###TEST CODE THAT WILL HOPEFULLY TEST EVERYTHING

###EVERYTHING NEEDED FOR BLINKING AND LED
import digitalio
import board
import time
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

####NEOPIXELS
import neopixel
pixel_brightness = 0.25
b = 30
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

####ANALOG LIGHT SENSOR
from analogio import AnalogIn
analogin = AnalogIn(board.A8)

###TEMPERATURE SENSOR
import adafruit_thermistor
thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

##Button Presses and Switch
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN
buttonB = digitalio.DigitalInOut(board.BUTTON_B)
buttonB.direction = digitalio.Direction.INPUT
buttonB.pull = digitalio.Pull.DOWN
switch = digitalio.DigitalInOut(board.SLIDE_SWITCH)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

##Accelerometer is hooked up to SDA/SCL which is I2C
import busio
import adafruit_lis3dh
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

###Play Sound
from audiocore import WaveFile,RawSample
from audiopwmio import PWMAudioOut as AudioOut
import math
import array
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
FREQUENCY = 440  # 440 Hz middle 'A'
SAMPLERATE = 8000  # 8000 samples/second, recommended!
# Generate one period of sine wav.
length = SAMPLERATE // FREQUENCY
sine_wave = array.array("H", [0] * length)
for i in range(length):
    sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
sine_wave_sample = RawSample(sine_wave)
def PLAY_BUZZER():
    with AudioOut(board.SPEAKER) as audio:
        audio.play(sine_wave_sample, loop=True)  # Play the single sine_wave sample continuously...
        time.sleep(1)  # for the duration of the sleep (in seconds)
        audio.stop()  # and then stop.
    return
def PLAY_WAV():
    filename = 'Feliz.wav'
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                pass
    return

##MICROPHONE
import audiobusio
NUM_SAMPLES = 160
num_bits = 16
mic = audiobusio.PDMIn(board.MICROPHONE_CLOCK, board.MICROPHONE_DATA,sample_rate=16000, bit_depth=num_bits)
samples = array.array('H', [0] * NUM_SAMPLES)
def normalized_rms(values):
    meanbuf = int(mean(values))
    samples_sum = sum(float(sample - meanbuf) * (sample - meanbuf) for sample in values)
    rms_mean = math.sqrt(samples_sum/len(values)) ##Notice that samples_sum = (sample-mean)**2
    return rms_mean
def mean(values):
    return sum(values) / len(values)

###VARS FOR BLINKING
BLINK = False
tblink = time.monotonic()
COLOR = 0

while True:
    ##READ SOUND
    mic.record(samples, len(samples))
    m = normalized_rms(samples)

    ###READ ACCELEROMETER
    x,y,z = lis3dh.acceleration

    ###CODE TO PRINT
    print('t:',time.monotonic(),' L:',analogin.value,' T(C): ',thermistor.temperature,' G(x,y,z):',x,y,z,' S:',m)
    time.sleep(0.1)

    ###Button Presses
    if buttonA.value == True:
        print("Button A pressed!!!")
        PLAY_WAV()
        while buttonA.value == True:
            pass
    if buttonB.value == True:
        print("Button B pressed!!!")
        PLAY_BUZZER()
        while buttonB.value == True:
            pass
    if switch.value == True:
        print("Switch Thrown!!!")
        while switch.value == True:
            pass

    ###BLINK CODE FOR BUILT-IN LED AND NEOPIXEL
    if time.monotonic() > tblink:
        if BLINK:
            led.value = True
            if COLOR == 0:
                pixels.fill((b,0,0)) #this will make all lights white
            if COLOR == 1:
                pixels.fill((b,0,b)) #this will make all lights white
            if COLOR == 2:
                pixels.fill((b,b,0)) #this will make all lights white
            if COLOR == 3:
                pixels.fill((0,b,b)) #this will make all lights white
            COLOR+=1
            if COLOR > 3:
                COLOR = 0
        else:
            led.value = False
            pixels.fill((0,0,0)) #this will make all lights white
        BLINK = not BLINK
        tblink += 1.0