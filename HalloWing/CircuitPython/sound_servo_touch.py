import board
import digitalio
import time
import touchio
import audioio

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

#Set up Touch Buttons
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

while True:
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
        Play_Sound()
    time.sleep(0.05)
    