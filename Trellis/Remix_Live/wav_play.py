##This will test playing a sounds from a WAV
import scipy.io.wavfile as S
import numpy as np
import os

VOICES = ["voice01.wav", "voice02.wav", "voice03.wav", "voice04.wav"]
for v in VOICES:
	fs, audio = S.read(v)
	S.write('test.wav',fs,audio)
	os.system('aplay test.wav')
