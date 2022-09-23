import time
import board
import busio
import digitalio
import math
import adafruit_MPU6050
import neopixel

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

def PLAY_SOUND(filename):
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                pass
    return

pixel_brightness = 0.25

#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

i2c_ext = busio.I2C(board.SCL, board.SDA)

sensor_ext = adafruit_MPU6050.MPU6050(i2c_ext,address=0x68)
#while not i2c_ext.try_lock():
#    pass

#try:
#    while True:
#        print("I2C addresses found:", [hex(device_address)
#              for device_address in i2c_ext.scan()])
#        time.sleep(2)

#finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
#    i2c_ext.unlock()
## 0x1e and 0x6a on my broken one
## 0x1c and 0x6a on a working one - 6a is the accelerometer/gyro and the 1e/1c is the magneometer

while True:
    #x,y,z = lis3dh.acceleration
    xe,ye,ze = sensor_ext.acceleration
    angle = -(math.atan2(xe,ze)*180/3.141592654-180)
    #gx,gy,gz = sensor_ext.gyro
    #bx,by,bz = mag.magnetic
    #print((bx,by,bz))
    #print((x,xe,y,ye,z,ze))
    #print((x,y,z))
    print((angle,))
    if abs(angle-43) > 15.0:
        pixels.fill((255,255,255)) #this will make all lights white
        print('HEY YO. THERE BE A FISH MATEY')
        PLAY_SOUND('r2d2-scream1.wav')
        time.sleep(1.0)
        pixels.fill((0,0,0))
    #print((gx,gy,gz))
    time.sleep(0.1)