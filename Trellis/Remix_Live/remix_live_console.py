###This is the console version of the python demo. You have a 
#4x8 array that you can turn on and off. The beat will play if one of the beats is on
#while the ticker moves across. I thought about creating a GUI but it's all done with just 
#the terminal

##################MODULES##################

import numpy as np
import time
import keyboard
import threading
import sys
import os
import pyxhook
from pydub import AudioSegment
from pydub.playback import play

###############GLOBALS##################

pixels = np.zeros([4,6])
KEY = []
audio_all = []
##And the column is dictated by Instrument
INSTRUMENTS = ["Drums","Tops","Bass","Chords","Leads","Voice"]
###Remember the type is the row
TYPES = ["Hey","Square","Deeper","Days"]

###############CLASSES##################

class MediaPlayer():
	def __init__(self):
		self.status = False
		self.running = True
		self.t1_stop = threading.Event()
		self.row = None
		self.col = None
		self.t1 = threading.Thread(target=self.player_thread)
		self.t1.start()

	def player_thread(self):
		while self.running == True:
			if self.status == False:
				self.play_sounds()
		print('Partys over')

	def play_sounds(self):
		global audio_all,INSTRUMENTS,TYPES
		if self.row is not None:
			#print row,col
			#So we need to iterate through the solution set
			ctr = 0
			##Combine with pydub module
			mixed = None
			for x in range(0,len(self.row)):
				r = self.row[x]
				c = self.col[x]
				if mixed is None:
					mixed = audio_all[c][r]
				else:
					mixed = mixed.overlay(audio_all[c][r])
			self.status = True
			play(mixed)
			self.status = False

	def stop(self):
		player.row = None
		player.col = None
		player.running = False
		print 'Stopping Player Thread. Waiting for song to end'
		self.t1_stop.set()

#################FUNCTIONS#################

def which_sounds():
	#Check to see if any of the pixels are on. If so we need to play that sound
	row = None
	col = None
	if 1 in pixels:
		row,col = np.where(pixels==1)
		for x in range(0,len(row)):
			r = row[x]
			c = col[x]
			i = INSTRUMENTS[c]
			t = TYPES[r]
			song_name = i + ' ' + t + '.ogg'
			print('Playing ',song_name)
	return row,col

def OnKeyPress(event):
	global KEY
	#print(event.Key)
	KEY = event.Key

def close_all():
	global pixels
	print('Killing Keylogger')
	hm.cancel()
	player.stop()
	#print('Quiting Program. Have a nice night.')
	sys.exit()

#Check which key in the key map was pressed
def check_key_map():
	global KEY
	##Create key map for the keyboard
	KEY_MAP = [['1','2','3','4','5','6'],['q','w','e','r','t','y'],['a','s','d','f','g','h'],['z','x','c','v','b','n']]
	for x in range(0,4):
		if KEY in KEY_MAP[x]:
			#Grab the column
			y = KEY_MAP[x].index(KEY)
			#Reset the KEY
			KEY = []
			#Turn pixels on or off depending on whether or not it's on or off
			button_press(x,y)

def button_press(x,y):
	global pixels
	#In this version of the trellis you need to turn off the sound if it's on
	if pixels[x,y] == 1:
		pixels[x,y] == 0
	else:
	#But if the sound is off you need to make sure there's only one from that column currently on
		for ri in range(0,4):
			pixels[ri,y] = 0
		pixels[x,y] = 1
	print(pixels)

##################SETUP####################

# create a hook manager for keyboard presses
hm = pyxhook.HookManager()
# watch for all keyboard presses
hm.KeyDown = OnKeyPress
# set the hook
hm.HookKeyboard()
# kick off the thread
hm.start()

#Kick off the player thread
player = MediaPlayer()

#Run Permutations on INSTRUMENTS AND TYPES
for i in INSTRUMENTS:
	audio_instrument = []
	for t in TYPES:	
		song_name = i + ' ' + t + '.ogg'
		print('Reading ',song_name)
		audio = AudioSegment.from_file('Future_Beat/' + song_name)
		audio_instrument.append(audio)
		#play(audio)
	audio_all.append(audio_instrument)

#Print current light status
print(pixels)

##################Main Loop#################

while True:
	#print the pixels too
	print('....')
	print(pixels)	
	##Check and see which sounds need to be played
	player.row,player.col = which_sounds()

	#The place where we actually play the songs is in the mediaplayer class 
	#which runs all the time. No need really to change anything
	
	##Check for Key press
	if len(KEY) > 0:
		#Check if one of the pixels is pressed
		#print('Checking Key Map')
		check_key_map()
		#Check for program quit
		if KEY == 'space':
			close_all()

	#Clear terminal
	os.system('clear')