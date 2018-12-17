import math
import time
import array
import board
import busio
import audioio
import adafruit_trellis_express
import adafruit_adxl34x
from wave_parsing import parse_wav

tempo = 180  # Starting BPM

# You can use the accelerometer to speed/slow down tempo by tilting!
ENABLE_TILT_TEMPO = True

# You get 4 voices, they must all have the same sample rate and must
# all be mono or stereo (no mix-n-match!)
VOICES = ["voice01.wav", "voice02.wav", "voice03.wav", "voice04.wav"]

# four colors for the 4 voices, using 0 or 255 only will reduce buzz
DRUM_COLOR = ((0, 255, 255),
              (0, 255, 0),
              (255, 255, 0),
              (255, 0, 0) )
# the color for the sweeping ticker
TICKER_COLOR = (255, 255, 255)

# Our keypad + neopixel driver
trellis = adafruit_trellis_express.TrellisM4Express(rotation=90)

# Our accelerometer
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
accelerometer = adafruit_adxl34x.ADXL345(i2c)

def wheel(pos): # Input a value 0 to 255 to get a color value.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    elif pos < 85:
        return(int(pos * 3), int(255 - pos*3), 0)
    elif pos < 170:
        pos -= 85
        return(int(255 - pos*3), 0, int(pos * 3))
    else:
        pos -= 170
        return(0, int(pos * 3), int(255 - pos*3))

# Play the welcome wav (if its there)
with audioio.AudioOut(board.A1, right_channel=board.A0) as audio:
    try:
        f = open("welcome.wav", "rb")
        wave = audioio.WaveFile(f)
        audio.play(wave)
        j = 0
	trellis.pixels._neopixel.brightness = 0.25
        while audio.playing:
            for i in range(32):
                pixel_index = (i * 256 // 32) + j
                trellis.pixels._neopixel[i] = wheel(pixel_index & 255)
            trellis.pixels._neopixel.show()
            j = (j+1) % 256
            time.sleep(0.005)
        f.close()
        # Clear all pixels
        trellis.pixels._neopixel.fill(0)
        trellis.pixels._neopixel.show()
	trellis.pixels._neopixel.brightness = 1.0
        # just hold a moment
        time.sleep(0.5)
    except OSError:
        # no biggie, they probably deleted it
        pass
        

# Parse the first file to figure out what format its in
wave_format = parse_wav(VOICES[0])
print(wave_format)

# Audio playback object - we'll go with either mono or stereo depending on
# what we see in the first file
if wave_format['channels'] == 1:
    audio = audioio.AudioOut(board.A1)
elif wave_format['channels'] == 2:
    audio = audioio.AudioOut(board.A1, right_channel=board.A0)
else:
    raise RuntimeError("Must be mono or stereo waves!")
mixer = audioio.Mixer(voice_count=4, sample_rate=wave_format['sample_rate'],
                     channel_count=wave_format['channels'],
                      bits_per_sample=16, samples_signed=True)
audio.play(mixer)

samples = []
# Read the 4 wave files, convert to stereo samples, and store
# (show load status on neopixels and play audio once loaded too!)
for v in range(4):
    trellis.pixels[(v, 0)] = DRUM_COLOR[v]
    wave_file = open(VOICES[v], "rb")
    # OK we managed to open the wave OK
    for x in range(1,4):
        trellis.pixels[(v, x)] = DRUM_COLOR[v]
    sample = audioio.WaveFile(wave_file)
    # debug play back on load!
    mixer.play(sample, voice=0)
    for x in range(4,7):
        trellis.pixels[(v, x)] = DRUM_COLOR[v]
    while mixer.playing:
        pass
    trellis.pixels[(v, 7)] = DRUM_COLOR[v]
    samples.append(sample)

# Clear all pixels
trellis.pixels._neopixel.fill(0)
trellis.pixels._neopixel.show()

# Our global state
current_step = 7 # we actually start on the last step since we increment first
# the state of the sequencer
beatset = [[False] * 8, [False] * 8, [False] * 8, [False] * 8]
# currently pressed buttons
current_press = set()

while True:
    stamp = time.monotonic()
    # redraw the last step to remove the ticker bar (e.g. 'normal' view)
    for y in range(4):
        color = 0
        if beatset[y][current_step]:
            color = DRUM_COLOR[y]
        trellis.pixels[(y, current_step)] = color

    # next beat!
    current_step = (current_step + 1) % 8
    
    # draw the vertical ticker bar, with selected voices highlighted
    for y in range(4):
        if beatset[y][current_step]:
            r, g, b = DRUM_COLOR[y]
            color = (r//2, g//2, b//2)  # this voice is enabled
            #print("Playing: ", VOICES[y])
            mixer.play(samples[y], voice=y)
        else:
            color = TICKER_COLOR     # no voice on
        trellis.pixels[(y, current_step)] = color

    # handle button presses while we're waiting for the next tempo beat
    # also check the accelerometer if we're using it, to adjust tempo
    while time.monotonic() - stamp < 60/tempo:
        # Check for pressed buttons
        pressed = set(trellis.pressed_keys)
        #print(pressed)
        for down in pressed - current_press:
            print("Pressed down", down)
            y = down[0]
            x = down[1]
            beatset[y][x] = not beatset[y][x] # enable the voice
            if beatset[y][x]:
                color = DRUM_COLOR[y]
            else:
                color = 0
            trellis.pixels[down] = color
        current_press = pressed

        if ENABLE_TILT_TEMPO:
            # Check accelerometer tilt!
            tilt = accelerometer.acceleration[1]
            #print("%0.1f" % tilt)
            new_tempo = tempo
            if tilt < -9:
                new_tempo = tempo + 5
            elif tilt < -6:
                new_tempo = tempo + 1
            elif tilt > 9:
                new_tempo = tempo - 5
            elif tilt > 6:
                new_tempo = tempo - 1
            if new_tempo != tempo:
                tempo = max(min(new_tempo, 300), 100)
                print("Tempo: %d BPM" % tempo)
                time.sleep(0.05)  # dont update tempo too fast!
        time.sleep(0.01)  # a little delay here helps avoid debounce annoyances


            
