import time
import board
import busio
import digitalio
import math
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

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

while True:
    print('oh')
    if buttonA.value:
        print('Hey')
        pixels.fill((255,255,255)) #this will make all lights white
        ##MUST BE 16-BIT 44,100 Hz - use AUDACITY TO EXPORT WAV
        PLAY_SOUND('Feliz.wav')
        time.sleep(1.0)
        pixels.fill((0,0,0))
    #print((gx,gy,gz))
    time.sleep(0.1)