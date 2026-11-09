import time
import board
import pwmio
from analogio import AnalogOut

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
[C4, dotted_quarter_note], [D4, eighth_note], [E4, whole_note],[0,whole_note]]

# Create piezo buzzer PWM output.
buzzer = pwmio.PWMOut(board.A1, variable_frequency=True)

# Start at the first note and start making sound.
buzzer.frequency = A3
buzzer.duty_cycle = 2**15  # 32768 value is 50% duty cycle, a square wave.

# Main loop will go through each tone in order up and down.
while True:
    print(time.monotonic())
    # Play tones going from start to end of list.
    for i in range(len(jingle_bells_song)):
        note,note_length = jingle_bells_song[i]
        print(note,note_length)
        if note == 0:
            buzzer.duty_cycle = 0
        else:
            buzzer.duty_cycle = 2**15
            buzzer.frequency = note
        time.sleep(note_length)
