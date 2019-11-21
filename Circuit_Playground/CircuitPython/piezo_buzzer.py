import time

import board
import pulseio
from analogio import AnalogOut
from adafruit_circuitplayground.express import cpx

# set up time signature
whole_note = 1.5  # adjust this to change tempo of everything
# these notes are fractions of the whole note
half_note = whole_note / 2
quarter_note = whole_note / 4
dotted_quarter_note = quarter_note * 1.5
eighth_note = whole_note / 8

# set up note values
A3 = 220
Bb3 = 233
B3 = 247
C4 = 262
Db4 = 277
D4 = 294
Eb4 = 311
E4 = 330
F4 = 349
Gb4 = 370
G4 = 392
Ab4 = 415
A4 = 440
Bb4 = 466
B4 = 494
# jingle bells
jingle_bells_song = [[E4, quarter_note], [E4, quarter_note],
[E4, half_note], [E4, quarter_note], [E4, quarter_note],
[E4, half_note], [E4, quarter_note], [G4, quarter_note],
[C4, dotted_quarter_note], [D4, eighth_note], [E4, whole_note]]
bpm = 100
# Define a list of tones/music notes to play.
TONE_FREQ = [ 262,  # C4
              294,  # D4
              330,  # E4
              0,
              349,  # F4
              392,  # G4
              440,  # A4
              494 ] # B4

length_relative = [0.5,
        1.0,
        1.5,
        1.0,
        0.4,
        2.3,
        0.8,
        3]

# Create piezo buzzer PWM output.
buzzer = pulseio.PWMOut(board.A1, variable_frequency=True)

# Start at the first note and start making sound.
buzzer.frequency = TONE_FREQ[0]
buzzer.duty_cycle = 2**15  # 32768 value is 50% duty cycle, a square wave.

# Main loop will go through each tone in order up and down.
while True:
    # Play tones going from start to end of list.
    n = 0
    for i in range(len(jingle_bells_song)):
        #cpx.start_tone(jingle_bells_song[i][0])
        n+=1
        if n >= len(TONE_FREQ):
            n = 0
        if TONE_FREQ[n] == 0:
            buzzer.duty_cycle = 0
        else:
            buzzer.duty_cycle = 2**15
            buzzer.frequency = TONE_FREQ[n]

        time.sleep(length_relative[i]/bpm)
        #cpx.stop_tone()