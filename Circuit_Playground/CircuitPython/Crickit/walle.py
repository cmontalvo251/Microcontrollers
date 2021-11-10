import time
import math as M
import board
import digitalio
import busio
import adafruit_lis3dh
from adafruit_crickit import crickit

# make two variables for the motors to make code shorter to type
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

##Setup Speaker
try:
    from audiocore import WaveFile
except ImportError:
    from audioio import WaveFile

try:
    from audioio import AudioOut
except ImportError:
    try:
        from audiopwmio import PWMAudioOut as AudioOut
    except ImportError:
        pass  # not always supported by every board!
# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True
squeals = ("evesurprised16bit.wav","r2d2-scream1.wav","eve16bit.wav","wall-e16bit.wav")

###Song Function
def PLAY_SONG(filename):
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                pass
    return

#Say hello Wall-E
print('I am Walle')
PLAY_SONG('wall-e16bit.wav')

##Initialize Counter
ctr = 0
##Speed of motors
speed = 0.5
while True:
    ##Read Accelerometer
    x,y,z = lis3dh.acceleration
    z-=9.81
    norm = M.sqrt(x**2+y**2+z**2)
    print((norm,))

    ###Go Forward
    motor_1.throttle = speed
    motor_2.throttle = speed

    ##Check for bonk
    if norm > 2:
        motor_1.throttle = 0.0
        motor_2.throttle = 0.0
        PLAY_SONG(squeals[ctr])
        ctr+=1
        if ctr >= len(squeals):
            ctr = 0
        #then back up for 2 seconds
        motor_1.throttle = -speed
        motor_2.throttle = -speed
        time.sleep(2)
        ##Then rotate for 0.5 seconds
        motor_1.throttle = speed
        motor_2.throttle = -speed
        time.sleep(0.5)
    ##Sleep
    time.sleep(0.1)