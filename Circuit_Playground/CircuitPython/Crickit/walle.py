import time
import math as M
import board
import digitalio
import busio
import adafruit_lis3dh
from adafruit_crickit import crickit

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
squeals = ("eveq16bit.wav","evesurprised16bit.wav","eve16bit.wav","r2d2-scream1.wav")

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
while True:
    ##Read Accelerometer
    x,y,z = lis3dh.acceleration
    z-=9.81
    norm = M.sqrt(x**2+y**2+z**2)
    print((norm,))
    ##Check for bonk
    if norm > 2:
        PLAY_SONG(squeals[ctr])
        ctr+=1
        if ctr >= len(squeals):
            ctr = 0
    ##Sleep
    time.sleep(0.1)