import board
import digitalio
import time
from analogio import AnalogIn,AnalogOut
import busio
import adafruit_lis3dh
import audioio

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

# enable the speaker
#print(dir(board))
#speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
#speaker_enable.direction = digitalio.Direction.OUTPUT
#speaker_enable.value = True

def Play_Sound():
    wavfile = "poke_center.wav"
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a = audioio.AudioOut(board.SPEAKER) #this is equivalent to board.A0
    a.play(wav)
    tstart = time.monotonic()
    tnext = 0.25
    while a.playing:
        #pass
        #print("Playing")
        if time.monotonic() > tstart + tnext:
            tstart += tnext
            big_led.value = not big_led.value
    big_led.value = False
    f.close()

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.LIGHT) ##this is equivalent to board.A1
while True:
    light = photocell.value*330/(2**16)
    print("Time (sec) = ",time.monotonic(),"Light (Lux) =",light)
    time.sleep(0.1)
    if light < 8:
       Play_Sound()
    