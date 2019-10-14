import board
import digitalio
import time
import touchio
import audioio
from analogio import AnalogIn,AnalogOut

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

a = audioio.AudioOut(board.SPEAKER) #this is equivalent to board.A0

def Play_Sound():
    wavfile = "poke_center.wav"
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
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

#Setup Button Activation
analog_button = AnalogIn(board.A2)

while True:
    if analog_button.value > 32000:
        print("Touched")
        Play_Sound()
    time.sleep(0.05)
    