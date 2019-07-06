##Import gaming engine environments
import ugame
import stage
import random
import analogio
import board

class Gravity():
	def __init__(self):
		self.joy_y = analogio.AnalogIn(ugame.board.JOYSTICK_X) #These are swapped to agree with 
		self.joy_x = analogio.AnalogIn(ugame.board.JOYSTICK_Y) #the TFT screen
		self.JOY_MIN = 100.0
		self.JOY_MAX = 65520.0
		self.GRAV = 2.0
		self.slope = -2.0*self.GRAV/(self.JOY_MAX-self.JOY_MIN)
	def print(self):
		self.compute()
		print(self.joy_x.value,self.joy_y.value,self.x,self.y)
	def compute(self):
		self.x = self.slope*(self.joy_x.value - self.JOY_MIN) + self.GRAV
		self.y = self.slope*(self.joy_y.value - self.JOY_MIN) + self.GRAV

class Ball():
	def __init__(self,bank):
		init_x = random.randint(16,144)
		init_y = random.randint(16,112)
		self.sprite = stage.Sprite(bank,1,init_x,init_y)
		self.speed_x = random.randint(-3,3)
		if self.speed_x == 0:
			self.speed_x = 1
		self.speed_y = random.randint(-3,3)
		if self.speed_y == 0:
			self.speed_y = 1
		self.check_frame_tick()
		self.frame_number = 1
		self.friction = 0.001
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
balls = []
num_sprites = 1
for x in range(0,num_sprites):
	ball = Ball(bank)
	balls.append(ball)
	sprites.append(ball.sprite)

##Create Text
s = "This game is ok"
text = stage.Text(len(s), 1)
text.move(32, 60)
text.text(s)

##Create game
game = stage.Stage(ugame.display,12) #the 12 is FPS
layers = [text] + sprites + background
game.layers = layers
game.render_block()

grav = Gravity()

#ball.frame is the current frame number
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
	game.render_sprites(sprites)
	game.tick()
