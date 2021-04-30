#I would like for it to play 4 different tones depending on the distance of the object.
#The sensor can go up to 400 cm. 
#I don't have a sonar sensor so I'm going to use the light sensor
import time
import board
from analogio import AnalogIn
from adafruit_circuitplayground.express import cpx

##Create the light sensor
light = AnalogIn(board.A8)

dt = 0.1
while True:
	##Light sensor can go between 0 and 65536 
    ##so let's convert it to 0 to 400
    val = 400.0*light.value/65536.0

    ##I don't want sounds to play all day so I am going to use the cpx switch to tell
    ##whether or not we hear the sound or not
    if cpx.switch == True
	    #I am thinking from 100 or less to play 262 Hz
	    if val < 100:
	    	cpx.play_tone(262,dt)
	    #between 101  and 200 to play 329 Hz
	    if val < 200 and val >= 101:
	    	cpx.play_tone(329,dt)
		#201 to 300 play 391 Hz,
		if val < 300 and val >= 201:
			cpx.play_tone(391,dt)
		#and 301 to 400 play 523 Hz.
		if val < 400 and val >= 301:
			cpx.play_tone(523,dt)
	#Print Light and Val
    print((light.value,val))
    time.sleep(0.1)