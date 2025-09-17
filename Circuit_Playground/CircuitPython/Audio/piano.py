# Example CircuitPython code for playing songs using cp.play_tone
# This code was written almost entirely (>90%) by ChatGPT-4.1
name = "Mary Had a Little Lamb"  # Change this to select the song

import time
import board
import simpleio
import digitalio

# Enable speaker on Circuit Playground Express
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True  # Turn on speaker

# Define note frequencies (Hz)
C4 = 262
D4 = 294
E4 = 330
F4 = 349
G4 = 392
A4 = 440
B4 = 494

# Define note durations (seconds)
QUARTER = 0.4
EIGHTH = 0.2

# Define songs as lists of (frequency, duration) tuples
mary_notes = [
    (E4, QUARTER), (D4, QUARTER), (C4, QUARTER), (D4, QUARTER),
    (E4, QUARTER), (E4, QUARTER), (E4, QUARTER),
    (D4, QUARTER), (D4, QUARTER), (D4, QUARTER),
    (E4, QUARTER), (G4, QUARTER), (G4, QUARTER),
    (E4, QUARTER), (D4, QUARTER), (C4, QUARTER), (D4, QUARTER),
    (E4, QUARTER), (E4, QUARTER), (E4, QUARTER), (E4, QUARTER),
    (D4, QUARTER), (D4, QUARTER), (E4, QUARTER), (D4, QUARTER), (C4, QUARTER)
]

yankee_notes = [
    (C4, EIGHTH), (C4, EIGHTH), (D4, EIGHTH), (E4, EIGHTH),
    (C4, EIGHTH), (E4, EIGHTH), (F4, EIGHTH), (G4, EIGHTH),
    (C4, EIGHTH), (G4, EIGHTH), (A4, EIGHTH), (G4, EIGHTH),
    (F4, EIGHTH), (E4, EIGHTH), (D4, EIGHTH), (C4, EIGHTH)
]

# Function to play a song
def play_song(notes):
    for freq, dur in notes:
        simpleio.tone(board.A0, freq, duration=dur)
        time.sleep(0.05)  # Short pause between notes

# Switch-case logic for song selection
if name == "Mary Had a Little Lamb":
    play_song(mary_notes)
elif name == "Yankee Doodle":
    play_song(yankee_notes)
else:
    print("Song not found")