###This is remix_live adapted from remix_live_console.py but altered to work on the trellis

#############MODULES#######################

import math
import time
import array
import struct
import board
import busio
import audioio
import adafruit_trellis_express
import adafruit_adxl34x

###########GLOBALS########################

###Remember the type is the row
TYPES = ["Hey","Square","Deeper","Days"]
##And the column is dictated by Instrument
INSTRUMENTS = ["Drums","Tops","Bass","Chords","Leads"]

#############FUNCTIONS#####################

def which_buttons(stamp,current_press):
	# handle button presses while we're waiting for the next tempo beat
	# also check the accelerometer if we're using it, to adjust tempo
	#while time.monotonic() - stamp < 60/tempo:
	# Check for pressed buttons
	print("Checking for pressed buttons")
	pressed = set(trellis.pressed_keys)
	for down in pressed - current_press:
		print("Pressed down", down)
		row = down[0]
		col = down[1]
		if beatset[row][col] == True:
			print("Turning Pixel off")
			#This means this pixel is already on so we just turn it off
			beatset[row][col] = False
			trellis.pixels[down] = (0,0,0)
		else:
			print("Turning Pixel On")
			#This means the pixel is off so we need to turn it on
			#but we need ot make sure no other pixels are off
			for ri in range(0,4):
				beatset[ri][col] = False
				trellis.pixels[(ri,col)] = (0,0,0)
			trellis.pixels[down] = COLUMN_COLOR[col]
			beatset[row][col] = True
	#Not so sure about this line of code but we shall see
	current_press = pressed
	return current_press

def play_songs(mixer,stamp,current_press):
	ctr = 0 
	for ri in range(0,4):
		if True in beatset[ri]:
			for ci in range(0,len(INSTRUMENTS)):
				if beatset[ri][ci] == True:
					i = INSTRUMENTS[ci]
					t = TYPES[ri]
					song_name = 'Future_Beat/' + i + ' ' + t + '000.wav'
					print('Playing ',song_name)
					if ctr < 4:
						mixer.play(samples[ci][ri],voice=ctr)
					else:
						print("Can only play 4 voices at once")
					ctr+=1
	while mixer.playing:
		current_press = which_buttons(stamp,current_press)
		print("...")
		time.sleep(0.25)

def parse_wav(filename,scale = 1.0):
	print("Reading file " + filename)
	f = open(filename, "rb")
	chunk_id = f.read(4)
	f.read(4) # skip chunksize
	format = f.read(4)
	subchunk1_id = f.read(4)
	if chunk_id != b'RIFF' or format != b'WAVE' or subchunk1_id != b'fmt ':
		raise RuntimeError("Invalid WAVE")
	subchunk1_size = struct.unpack("<I", f.read(4))[0]
	if subchunk1_size != 16:
		raise RuntimeError("Only PCM supported")
	audio_format, num_channels, sample_rate = struct.unpack("<HHI", f.read(8))
	if audio_format != 1:
		raise RuntimeError("Only uncompressed WAVs")
	f.read(6) # skip byterate and blockalign
	bits_per_sample = struct.unpack("<H", f.read(2))[0]
	subchunk2_id =  f.read(4)
	if subchunk2_id != b'data':
		raise RuntimeError("Invalid WAVE")
	data_size = struct.unpack("<I", f.read(4))[0]
	if bits_per_sample != 16:
		raise RuntimeError("Only 16 bit samples")
	num_samples = data_size * 8 // bits_per_sample
	f.close()
	return {'sample_rate': sample_rate,
			'channels': num_channels,
			'num_samples': num_samples,
			'data_size': data_size}

##################INITIAL ROUTINES###########################

# Parse the first file to figure out what format its in
# Audio must be 22050 Hz at 705 kbps stereo 16 bit uncompressed wav
# Use the command to convert audio files to the right format
wave_format = parse_wav('Future_Beat/' + INSTRUMENTS[0] + ' ' + TYPES[0] + '000.wav')
print(wave_format)

# Audio playback object - we'll go with either mono or stereo depending on
# what we see in the first file
if wave_format['channels'] == 1:
	audio = audioio.AudioOut(board.A1)
elif wave_format['channels'] == 2:
	audio = audioio.AudioOut(board.A1, right_channel=board.A0)
else:
	raise RuntimeError("Must be mono or stereo waves!")
mixer = audioio.Mixer(voice_count=4, sample_rate=wave_format['sample_rate'],
					 channel_count=wave_format['channels'],
					  bits_per_sample=16, samples_signed=True)
audio.play(mixer)

##Read in all files
samples = []
#Run Permutations on INSTRUMENTS AND TYPES
for i in INSTRUMENTS:
	samples_row = []
	for t in TYPES:	
		#i = INSTRUMENTS[0]
		#t = TYPES[0]
		song_name = i + ' ' + t + '000.wav'
		print('Reading ',song_name)
		wave_file = open('Future_Beat/' + song_name,"rb")
		sample = audioio.WaveFile(wave_file)
		#mixer.play(sample,voice=0)
		#while mixer.playing:
		#	pass
		samples_row.append(sample)
	samples.append(samples_row)

# colors for each column
COLUMN_COLOR = ((255, 0, 0),
			  (255, 0, 255),
			  (255, 255, 0),
			  (0, 255, 0),
			  (0, 0, 255),
			  (255,255,255),
			  (255,0,0),
			  (0,255,0))


tempo = 180  # Starting BPM

# Our keypad + neopixel driver
trellis = adafruit_trellis_express.TrellisM4Express(rotation=90)

# Clear all pixels
trellis.pixels._neopixel.fill(0)
trellis.pixels._neopixel.show()

# currently pressed buttons
current_press = set()

# the state of the sequencer
beatset = [[False] * 8, [False] * 8, [False] * 8, [False] * 8]

#ctr = 0
while True:
	#Get timestamp
	stamp = time.monotonic()

	#Check and see which songs to play
	play_songs(mixer,stamp,current_press)
	
	#Heartbeat				
	print("...")

	#Button check
	current_press = which_buttons(stamp,current_press)

	time.sleep(0.01)
