##Import gaming engine environments
import ugame
import stage
import random
import analogio
import board
import time

class Gravity():
	def __init__(self):
		self.joy_y = analogio.AnalogIn(ugame.board.JOYSTICK_X) #These are swapped to agree with 
		self.joy_x = analogio.AnalogIn(ugame.board.JOYSTICK_Y) #the TFT screen
		self.JOY_MIN = 100.0
		self.JOY_MAX = 65520.0
		self.GRAV = 0.5
		self.slope = -2.0*self.GRAV/(self.JOY_MAX-self.JOY_MIN)
	def print(self):
		self.compute()
		print(self.joy_x.value,self.joy_y.value,self.x,self.y)
	def compute(self):
		self.x = self.slope*(self.joy_x.value - self.JOY_MIN) + self.GRAV
		self.y = self.slope*(self.joy_y.value - self.JOY_MIN) + self.GRAV

class GoalPost():
	def __init__(self,bank,index,goalwidth):
		#For the stationary we need to pick a wall (left,right,top,bottom)
		#First let's run a random number generator to determine x or y
		self.LR_TP = random.randint(1,2)
		if self.LR_TP == 1:
			#print("Left or Right")
			##Left or right
			##Since we know it's left or right we need to pick the y coordinate
			init_y1 = random.randint(16,112)
			init_y2 = init_y1 + goalwidth
			self.ycoord = [init_y1,init_y2]
			if init_y2 > 112:
				init_y2 = init_y1 - goalwidth
				self.ycoord = [init_y2,init_y1]
			#Then we decide left or right
			self.LR = random.randint(1,2)
			if self.LR == 1:
				#print("LEFT")
				init_x1 = 2
			else:
				#print("RIGHT")
				init_x1 = 142
			init_x2 = init_x1
		else:
			#print("Top of Bottom")
			##Top or Bottom
			init_x1 = random.randint(16,144)
			init_x2 = init_x1 + goalwidth
			self.xcoord = [init_x1,init_x2] 
			if init_x2 > 144:
				init_x2 = init_x1 - goalwidth
				self.xcoord = [init_x2,init_x1]
			self.TP = random.randint(1,2)
			if self.TP == 1:
				#print("TOP")
				init_y1 = 2
			else:
				#print("BOTTOM")
				init_y1 = 110
			init_y2 = init_y1
		self.post1 = stage.Sprite(bank,index,init_x1,init_y1)
		self.post2 = stage.Sprite(bank,index,init_x2,init_y2)

class Ball():
	def __init__(self,bank):
		self.sprite = stage.Sprite(bank,1,0,0)
		self.Reset()
		self.friction = 0.001
	def Reset(self):
		self.stop = 0
		self.speed_x = random.randint(-3,3)
		if self.speed_x == 0:
			self.speed_x = 1
		self.speed_y = random.randint(-3,3)
		if self.speed_y == 0:
			self.speed_y = 1
		self.check_frame_tick()
		self.frame_number = 1
		init_x = random.randint(16,144)
		init_y = random.randint(16,112)
		self.sprite.move(init_x,init_y)
	def check_frame_tick(self):
		if self.speed_x > 0 and self.speed_y > 0:
			self.frame_tick = -1
		else:
			self.frame_tick = 1
	def tick_frame(self):
		self.frame_number += self.frame_tick
		if self.frame_number == 5:
			self.frame_number = 1
		if self.frame_number == 0:
			self.frame_number = 4
		self.sprite.set_frame(self.frame_number)
	def integrate(self,grav_x,grav_y):
		if self.stop == 0:
			##Create new x and y coordinate
			new_x = self.sprite.x + self.speed_x
			new_y = self.sprite.y + self.speed_y
			#Move once
			self.sprite.move(new_x,new_y)
			#Check for clipping and change speed
			if self.sprite.x < 0 or self.sprite.x > 144:
				self.speed_x = -self.speed_x
			if self.sprite.y < 0 or self.sprite.y > 112:
				self.speed_y = -self.speed_y
			#And then move again to make sure it does not go out of bounds
			if new_x > 144:
				new_x = 144
			elif new_x < 0:
				new_x = 0
			if new_y > 112:
				new_y = 112
			elif new_y < 0:
				new_y = 0
			self.sprite.move(new_x,new_y)
			#Since we just changed speeds go ahead and check frame tick
			self.check_frame_tick()
			#Compute Acceleration
			self.accel_x = self.friction*self.speed_x + grav_x
			self.accel_y = self.friction*self.speed_y + grav_y
			#Compute new speed
			self.speed_x -= self.accel_x
			self.speed_y -= self.accel_y
	def goal(self,posts):
		score = 0 ##score need to be 2 to win
		if posts.LR_TP == 1:
			#print("Left or Right")
			##Left or right
			if self.sprite.y < posts.ycoord[1] and self.sprite.y > posts.ycoord[0]:
				score+=1
			if posts.LR == 1:
				#print("LEFT")
				if self.sprite.x < posts.post1.x:
					score+=1
			else:
				if self.sprite.x > posts.post1.x:
					score+=1
				#print("RIGHT")
		else:
			if self.sprite.x < posts.xcoord[1] and self.sprite.x > posts.xcoord[0]:
				score+=1
			#print("Top of Bottom")
			##Top or Bottom
			if posts.TP == 1:
				#print("TOP")
				if self.sprite.y < posts.post1.y:
					score+=1
			else:
				if self.sprite.y > posts.post1.y:
					score+=1
				#print("BOTTOM")
		return score

##########KICK OFF THINGS THAT ARE CONSTANT BETWEEN LEVELS############

#Create the bank by importing
bank = stage.Bank.from_bmp16("sprites.bmp")

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
balls = []
num_sprites = 1 ##also will be used as the current level
current_level = 1
for x in range(0,num_sprites):
	ball = Ball(bank)
	balls.append(ball)
	sprites.append(ball.sprite)

#Make two goal posts
posts = GoalPost(bank,5,32)
sprites.append(posts.post1)
sprites.append(posts.post2)

##Create Text
s = "          "
win_text = stage.Text(len(s), 1)
win_text.move(32, 60)
win_text.text(s)

##Create game
game = stage.Stage(ugame.display,12) #the 12 is FPS
layers = [win_text] + sprites + background
game.layers = layers
game.render_block()

grav = Gravity()

#ball.frame is the current frame number
game_reset = False
while True:
	#To make the ball spin we can increase the frame in 
	#the bank from 1-4
	#There is a fancy way to do this with a modulo but
	#I'd rather do it the hacky way
	for ball in balls:
		#Update sprite
		ball.sprite.update()
		#Tick Frame
		ball.tick_frame()
		#Compute Gravity
		grav.compute()
		##Change Speed and position
		ball.integrate(grav.x,grav.y)
		#Check for goal
		score = ball.goal(posts)
		#print(score)
		if score == 2 and ball.stop == 0:
			num_sprites -= 1
			ball.stop = 1
			if num_sprites == 0:
				win_text.text("YOU WIN!!!")
				game.render_block()
				#Wait 2 seconds after you win before resetting the game
				time.sleep(2)
				game_reset = True
	#Ok so what to do when the game gets reset?
	if game_reset == True:
		game_reset = False
		#First increase the level counter
		current_level += 1
		#Then reset the number of sprites
		num_sprites = current_level
		#Now we can reset all the balls position and velocity
		for ball in balls:
			ball.Reset()
		#Finally, add a sprite to the sprites and balls
		ball = Ball(bank)
		balls.append(ball)
		sprites.append(ball.sprite)
		#Change the text
		win_text.text("          ")
		#We also need to reset the layers
		layers = [win_text] + sprites + background
		game.layers = layers
		game.render_block()

	game.render_sprites(sprites)
	game.tick()
