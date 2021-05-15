#I would like for it to play 4 different tones depending on the distance of the object.
#The sensor can go up to 400 cm.
#I don't have a sonar sensor so I'm going to use the light sensor
import time
import board
from adafruit_circuitplayground.express import cpx

dt = 0.1
while True:
	##Light sensor can go between 0 and 65536
    ##so let's convert it to 0 to 400
    val = 400.0*cpx.light/330.0

    ##I don't want sounds to play all day so I am going to use the cpx switch to tell
    ##whether or not we hear the sound or not
    if cpx.switch == True:
        if val < 100:
            cpx.play_tone(262,dt)
            print('262')
        if val >= 101 and val < 200:
            print('329')
            cpx.play_tone(329,dt)
        if val >= 201 and val < 300:
            print('420')
            cpx.play_tone(420,dt)
        if val >= 301 and val < 400:
            print('520')
            cpx.play_tone(520,dt)
	#Print Light and Val
    print((cpx.light,val))
    time.sleep(0.1)