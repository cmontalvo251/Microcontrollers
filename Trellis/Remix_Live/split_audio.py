###This will split all the audio files into 4 second tracks.


import os
import sys
import struct

###Remember the type is the row
TYPES = ["Hey","Square","Deeper","Days"]
##And the column is dictated by Instrument
INSTRUMENTS = ["Drums","Tops","Bass","Chords","Leads","Voice"]

#Run Permutations on INSTRUMENTS AND TYPES
for i in INSTRUMENTS:
	for t in TYPES:	
		in_name = "'Future_Beat/FFMPEG/cleaned/" + i + ' ' + t
		out_name = "'Future_Beat/FFMPEG/cleaned/Split/" + i + ' ' + t
		print('Reading ',in_name)
		#ffmpeg -i 'Bass Days.wav' -f segment -segment_time 4 -c copy out%03d.wav
		ffmpeg_cmd = 'ffmpeg -i ' + in_name + ".wav' -f segment -segment_time 4 -c copy " + out_name + "%03d.wav'"
		print(ffmpeg_cmd)
		os.system(ffmpeg_cmd)


