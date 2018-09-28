import audioio
import board
from adafruit_crickit import crickit

a = audioio.AudioOut(board.A0)

def PLAY_SOUND(wavfile):
    print("Playing Sound",wavfile)
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    # You can now do all sorts of stuff here while the audio plays
    # such as move servos, motors, read sensors...
    # Or wait for the audio to finish playing:
    while a.playing:
        pass
    f.close()
    return

wavfiles = ("r2d2-squeaks1.wav","r2d2-squeaks5.wav","r2d2-scream1.wav")
counter = 0
while True:
    if crickit.touch_1.value:
        print("Touched Cap Touch Pad 1")
        PLAY_SOUND(wavfiles[counter])
        counter+=1
        if counter > 2:
            counter = 0
    