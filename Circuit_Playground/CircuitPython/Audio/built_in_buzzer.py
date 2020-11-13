import time
import array
import math
import board
import digitalio
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
    time.sleep(1)  # for the duration of the sleep (in seconds)
    audio.stop()  # and then stop.

while True:
    print(a.value)
    time.sleep(0.1)
    if a.value == True:
        play_sound()
    #play_sound()