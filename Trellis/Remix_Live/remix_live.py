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

#############FUNCTIONS#####################

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

def wheel(pos): # Input a value 0 to 255 to get a color value.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    elif pos < 85:
        return(int(pos * 3), int(255 - pos*3), 0)
    elif pos < 170:
        pos -= 85
        return(int(255 - pos*3), 0, int(pos * 3))
    else:
        pos -= 170
        return(0, int(pos * 3), int(255 - pos*3))

##################INITIAL ROUTINES###########################

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

###Remember the type is the row
TYPES = ["Hey","Square","Deeper","Days"]
##And the column is dictated by Instrument
INSTRUMENTS = ["Drums","Tops","Bass","Chords","Leads","Voice"]

ctr = 0
while True:
	#Get timestamp
	stamp = time.monotonic()

	#Check and see which songs to play
	play_row = None
	play_col = None
	if True in beatset:
		print("Playing Songs....")
		#row,col = np.where(pixels==1)
		#for x in range(0,len(row)):
		#	r = row[x]
		#	c = col[x]
		#	i = INSTRUMENTS[c]
		#	t = TYPES[r]
		#	song_name = i + ' ' + t + '.ogg'
		#	print('Playing ',song_name)
	print("...")
	#for row in beatset:
	#	print(row)

	#for ri in range(0,4):
	#	for ci in range(0,6):
	#		print(trellis.pixels[(ri,ci)])
	#		#print(trellis.pixels[(ri,ci)])
	#		print(" ")

	# handle button presses while we're waiting for the next tempo beat
	# also check the accelerometer if we're using it, to adjust tempo
	while time.monotonic() - stamp < 60/tempo:
		# Check for pressed buttons
		pressed = set(trellis.pressed_keys)
    	for down in pressed - current_press:
    		print("Pressed down", down)
    		row = down[0]
    		col = down[1]
    		if beatset[row][col] == True:
    			print("Turning Pixel off")
    			#This means this pixel is already on so we just turn it off
    			beatset[row][col] = False
    		else:
    			print("Turning Pixel On")
    			#This means the pixel is off so we need to turn it on
    			#but we need ot make sure no other pixels are off
    			for ri in range(0,4):
    				beatset[ri][col] = False
    			beatset[row][col] = True
        #Not so sure about this line of code but we shall see
        current_press = pressed

	#This is where we are going to turn on pixels depending on the color
	for ri in range(0,4):
		for ci in range(0,6):
			if beatset[ri][ci] == True:
				trellis.pixels[(ri,ci)] = COLUMN_COLOR[ci]
			else:
				trellis.pixels[(ri,ci)] = (0,0,0)

	ctr+=1
	#print(ctr)
	time.sleep(0.01)