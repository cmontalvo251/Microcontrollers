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
background = [stage.Grid(bank,10,8)]
#Each tile in ball.bmp is 16x16 pixels
#A 10x8 grid is thus 160x128 pixels.
#That's the size of the TFT grid on the PyGamer
#print("Background Grid created")

##Create A Sprite. In this case a ball
#1 is the second slot in the bank and the position
#is 8,8
sprites = []
sprites.append(stage.Sprite(bank,1,30,8))
sprites.append(stage.Sprite(bank,1,50,50))

#sprites = [ball]

##Create game
game = stage.Stage(ugame.display,12) #the 12 is FPS
#print("Created Game")
layers = sprites + background
game.layers = layers
#game.layers = [ball,background]
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
#ball.frame is the current frame number
current_frame = sprites[0].frame
#current_frame = ball.frame
while True:
	#To make the ball spin we can increase the frame in 
	#the bank from 1-4
	#There is a fancy way to do this with a modulo but
	#I'd rather do it the hacky way
	current_frame+=1
	if current_frame == 5:
		current_frame = 1
	#sprites[0].set_frame(current_frame)
	#game.render_sprites(sprites)
	#ball.set_frame(ball.frame % 4 + 1)
	for sprite in sprites:
		sprite.set_frame(current_frame)
	game.render_sprites(sprites)
	game.tick()

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