import audioio
import board

wavfile = "r2d2-squeaks1.wav"
f = open(wavfile, "rb")
wav = audioio.WaveFile(f)
a = audioio.AudioOut(board.A0)
a.play(wav)
 
# You can now do all sorts of stuff here while the audio plays
# such as move servos, motors, read sensors...
# Or wait for the audio to finish playing:

while a.playing:
    pass
    
f.close()