import time
import random
import usb_midi
import adafruit_midi

midi = adafruit_midi.MIDI(midi_out=usb_midi.ports[1], out_channel=0)

print("Midi test")

# Convert channel numbers at the presentation layer to the ones musicians use
print("Default output channel:", midi.out_channel + 1)
print("Listening on input channel:",
      midi.in_channel + 1 if midi.in_channel is not None else None)

while True:
    midi.note_on(44, 120)
    midi.note_off(44, 120)
    midi.control_change(3, 44)
    midi.pitch_bend(random.randint(0, 16383))
    time.sleep(1)
