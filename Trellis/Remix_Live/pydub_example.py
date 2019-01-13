from pydub import AudioSegment
from pydub.playback import play

audio1 = AudioSegment.from_file("voice01.wav") #your first audio file
audio2 = AudioSegment.from_file("voice02.wav") #your second audio file
audio3 = AudioSegment.from_file("voice03.wav") #your third audio file
audio4 = AudioSegment.from_file("voice04.wav")

mixed = audio1.overlay(audio2)          #combine , superimpose audio files
mixed1  = mixed.overlay(audio3)          #Further combine , superimpose audio files
mixed2 = mixed1.overlay(audio4)

#If you need to save mixed file
mixed2.export("mixed.wav", format='wav') #export mixed  audio file
play(mixed2) 