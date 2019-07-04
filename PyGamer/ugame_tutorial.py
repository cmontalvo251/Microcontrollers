##Import gaming engine environments
import ugame
import stage
#When you run the game engine 
#the print statements overwrite the 
#render block so you need to use text blocks
#but I just put these here for tutorials sake
#print("Modules Loaded") 

#Create the bank by importing
bank = stage.Bank.from_bmp16("ball.bmp")
#print("Imported Bank BMP")

#Create a grid that's 10 x 8
background = stage.Grid(bank,10,8)
#Each tile in ball.bmp is 16x16 pixels
#A 10x8 grid is thus 160x128 pixels.
#That's the size of the TFT grid on the PyGamer
#print("Background Grid created")

##Create A Sprite. In this case a ball
ball = stage.Sprite(bank,1,8,8)

##Create game
game = stage.Stage(ugame.display,12) #the 12 is FPS
#print("Created Game")
game.layers = [ball,background]
#print("Layers Imported")
game.render_block()


####### USE THESE FOR BLINK CODE
#import board
#import digitalio
#import time
 
#led = digitalio.DigitalInOut(board.D13)
#led.direction = digitalio.Direction.OUTPUT
##################################3
 
ctr = 0

while True:
	pass

	#####THIS STUFF IF ONLY NEEDED FOR THE BLINK CODE###
	#ctr+=1
	#led.value = True
	#time.sleep(0.5)
	#led.value = False
	#time.sleep(0.5)
	#led.value = True
	#time.sleep(0.1)
	#led.value = False
	#time.sleep(0.1)
	#print("Hello World =",ctr)
	#################################################