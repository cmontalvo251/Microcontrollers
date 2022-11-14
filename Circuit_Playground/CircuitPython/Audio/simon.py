import time
import board
import busio
import digitalio
import adafruit_lis3dh
import math
import random
import neopixel

import time
import board
import pwmio

# Define a list of tones/music notes to play.
TONE_FREQ = [ 262,  # C4
              294,  # D4
              330,  # E4
              349,  # F4
              392,  # G4
              440,  # A4
              494 ] # B4

# Create piezo buzzer PWM output.
buzzer = pwmio.PWMOut(board.A4, variable_frequency=True)

# Start at the first note and start making sound.
buzzer.frequency = TONE_FREQ[0]
print(dir(buzzer))
buzzer.duty_cycle = 2**15  # 32768 value is 50% duty cycle, a square wave.

REGION_LEDS = (
    (5, 6, 7),  # yellow region
    (2, 3, 4),  # blue region
    (7, 8, 9),  # red region
    (0, 1, 2),  # green region
)

REGION_COLOR = (
    (255, 255, 0),  # yellow region
    (0, 0, 255),    # blue region
    (255, 0, 0),    # red region
    (0, 255, 0),    # green region
)

PAD_REGION = {
    'YELLOW': 0,  # yellow region 'A1'
    'RED': 2,  # red region 'A2/A3'
    'GREEN': 3,  # green region 'A4/A5'
    'BLUE': 1,  # blue region 'A6/TX'
}
##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

pixel_brightness = 0.25
#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness,auto_write=True)

def light_region(region, duration=1):
    # turn the LEDs for the selected region on
    for led in REGION_LEDS[region]:
        pixels[led] = REGION_COLOR[region]


    # play a tone for the selected region
    #cpb.start_tone(REGION_TONE[region])
    buzzer.duty_cycle = 2**15
    buzzer.frequency = TONE_FREQ[0]

    # wait the requested amount of time
    time.sleep(duration)

    # stop the tone
    buzzer.duty_cycle = 0

    # turn the LEDs for the selected region off
    for led in REGION_LEDS[region]:
        pixels[led] = (0, 0, 0)



def read_region(timeout=30):
    val = 0
    start_time = time.time()
    while time.time() - start_time < timeout:
        x,y,z = lis3dh.acceleration
        angle_cpb = math.atan2(x,z)*180/3.141
        angle_cpb2 = math.atan2(y,z)*180/3.141
        if abs(angle_cpb) < 4.0:
            angle_cpb = 0
        if abs(angle_cpb2) < 4.0:
            angle_cpb2 = 0
        print((angle_cpb,angle_cpb2))
        if abs(angle_cpb2) + abs(angle_cpb) > 8:
            if angle_cpb < 0 and angle_cpb2 < 0:
                val = PAD_REGION['RED']
                time.sleep(3.0)
                break
            elif angle_cpb < 0 and angle_cpb2 > 0:
                val = PAD_REGION['YELLOW']
                time.sleep(3.0)
                break
            elif angle_cpb >0 and angle_cpb2 > 0:
                val = PAD_REGION['BLUE']
                time.sleep(3.0)
                break
            elif angle_cpb > 0 and angle_cpb2 <0:
                val = PAD_REGION['GREEN']
                time.sleep(3.0)
                break
    return val


def play_sequence(sequence):
    duration = 1 - len(sequence) * 0.05
    if duration < 0.1:
        duration = 0.1
    for region in sequence:
        light_region(region, duration)

def read_sequence(sequence):
    for region in sequence:
        if read_region() != region:
            # the player made a mistake!
            return False
        light_region(region, 0.25)
    return True

def play_error():
    print('You lost')
    time.sleep(1)

sequence = []
while True:
    print('Top of While Loop')
    sequence.append(random.randint(0,3))
    print('Playing Sequence')
    buzzer.duty_cycle = 2**15
    buzzer.frequency = TONE_FREQ[1]
    # wait the requested amount of time
    time.sleep(2.0)
    # stop the tone
    buzzer.duty_cycle = 0
    play_sequence(sequence)
    print('Done playing sequence')
    if not read_sequence(sequence):
        play_error()
        break
        time.sleep(1.0)
    if len(sequence) >= 3:
        print("You won")
        break