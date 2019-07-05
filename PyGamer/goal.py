##Import gaming engine environments
import ugame
import stage
import random

#Create the bank by importing
bank = stage.Bank.from_bmp16("ball.bmp")

#Create a grid that's 10 x 8
background = [stage.Grid(bank,10,8)]
#Each tile in ball.bmp is 16x16 pixels
#A 10x8 grid is thus 160x128 pixels.
#That's the size of the TFT grid on the PyGamer
#print("Background Grid created")

##Create A Sprite. In this case a ball
#1 is the second slot in the bank and the position
# is the next 2 arguments which I use as random numbers
# so the game is different every time
sprites = []
speed_x = []
speed_y = []
frame_tick = []
frame_number = []
num_sprites = 3
for x in range(0,num_sprites):
	init_x = random.randint(16,144)
	init_y = random.randint(16,112)
	sprites.append(stage.Sprite(bank,1,init_x,init_y))
	speed_x.append(random.randint(-3,3))
	if speed_x[-1] == 0:
		speed_x[-1] = 1
	speed_y.append(random.randint(-3,3))
	if speed_y[-1] == 0:
		speed_y[-1] = 1
	if speed_x[-1] > 0 and speed_y[-1] > 0:
		frame_tick.append(-1)
	else:
		frame_tick.append(1)
	frame_number.append(1)

##Create Text
s = "This game sucks"
text = stage.Text(len(s), 1)
text.move(32, 60)
text.text(s)

##Create game
game = stage.Stage(ugame.display,12) #the 12 is FPS
layers = [text] + sprites + background
game.layers = layers
game.render_block()

#ball.frame is the current frame number
while True:
	#To make the ball spin we can increase the frame in 
	#the bank from 1-4
	#There is a fancy way to do this with a modulo but
	#I'd rather do it the hacky way
	ctr = 0
	for sprite in sprites:
		sprite.update()
		frame_number[ctr] += frame_tick[ctr]
		if frame_number[ctr] == 5:
			frame_number[ctr] = 1
		if frame_number[ctr] == 0:
			frame_number[ctr] = 4
		sprite.set_frame(frame_number[ctr])
		##Create new x and y coordinate
		new_x = sprite.x + speed_x[ctr]
		new_y = sprite.y + speed_y[ctr]
		sprite.move(new_x,new_y)
		if sprite.x < 0 or sprite.x > 144:
			speed_x[ctr] = -speed_x[ctr]
		if sprite.y < 0 or sprite.y > 112:
			speed_y[ctr] = -speed_y[ctr]
		if speed_x[ctr] > 0 and speed_y[ctr] > 0:
			frame_tick[ctr] = -1
		else:
			frame_tick[ctr] = 1
		ctr+=1
	game.render_sprites(sprites)
	game.tick()
