import audioio
import board
import digitalio

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True

wavfile = "darth.wav"
f = open(wavfile, "rb")
wav = audioio.WaveFile(f)
a = audioio.AudioOut(board.A0)
a.play(wav)
 
# You can now do all sorts of stuff here while the audio plays
# such as move servos, motors, read sensors...
# Or wait for the audio to finish playing:

while a.playing:
    print("Playing")
    pass
    
f.close()