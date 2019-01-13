###This is the console version of the python demo. You have a 
#4x8 array that you can turn on and off. The beat will play if one of the beats is on
#while the ticker moves across. I thought about creating a GUI but it's all done with just 
#the terminal

##################MODULES##################

import numpy as np
import time
import keyboard
import sys
import os
import pyxhook

###############GLOBALS##################

KEY = []
pixels = np.zeros([4,8])

#################FUNCTIONS#################

def OnKeyPress(event):
	global KEY
	#print(event.Key)
	KEY = event.Key

def button_press(x,y):
	global pixels
	if pixels[x,y] == 0 or pixels[x,y] == 9:
		pixels[x,y] += 1
	else:
		pixels[x,y] -= 1

#Check which key in the key map was pressed
def check_key_map():
	global KEY
	##Create key map for the keyboard
	KEY_MAP = [['1','2','3','4','5','6','7','8'],['q','w','e','r','t','y','u','i'],['a','s','d','f','g','h','j','k'],['z','x','c','v','b','n','m',',']]
	for x in range(0,4):
		if KEY in KEY_MAP[x]:
			#Grab the column
			y = KEY_MAP[x].index(KEY)
			#Reset the KEY
			KEY = []
			#Turn pixels on or off depending on whether or not it's on or off
			button_press(x,y)

def close_all():
	print('Quiting Program. Have a nice night.')
	hm.cancel()
	sys.exit()

def which_sounds():
	#Check to see if any of the pixels are 10. If so we need to play that sound
	row = None
	if 9 in pixels:
		#In this case we just care about the row but let's get both
		row,col = np.where(pixels==9)
		#print(row)
	#If the routine above returns a row object we need to figure out what songs to play and play them
	if row is not None:
		sounds = []
		for r in row:
			sounds.append(VOICES[r])
	else:
		sounds = None

	return sounds

def play(sounds):
	if sounds is not None:
		print(sounds)

##################VARIABLES###############

ticker = np.zeros(4) + 8
ticker_col = -1
tempo = 240 ##beat per minute

##################SETUP####################

sleep_time = 1./(tempo/60.)
# create a hook manager for keyboard presses
hm = pyxhook.HookManager()
# watch for all keyboard presses
hm.KeyDown = OnKeyPress
# set the hook
hm.HookKeyboard()
# kick off the thread
hm.start()

# You get 4 voices, they must all have the same sample rate and must
# all be mono or stereo (no mix-n-match!)
VOICES = ["voice01.wav", "voice02.wav", "voice03.wav", "voice04.wav"]

##################Main Loop#################

while True:
	#Advance the ticker
	ticker_col+=1 
	if ticker_col > 7:
		ticker_col = 0

	#Plot the ticker
	pixels[:,ticker_col] = pixels[:,ticker_col] + ticker

	#Print the current pixel arrangement
	print(pixels)

	#Pause for a moment depending on tempo
	time.sleep(sleep_time)

	#Check to see which sounds need to be played
	sounds = which_sounds()

	#Play sounds if any
	play(sounds)
	
	#Remove the ticker
	pixels[:,ticker_col] = pixels[:,ticker_col] - ticker

	##Check for Key press
	if len(KEY) > 0:
		#Check if one of the keys turns on a sound
		check_key_map()
		#Check for program quit
		if KEY == 'space':
			close_all()
