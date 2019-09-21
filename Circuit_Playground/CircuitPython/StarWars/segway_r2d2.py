from adafruit_circuitplayground.express import cpx
import math
import time
from adafruit_crickit import crickit
import audioio
import board
import neopixel
import digitalio
 
# make two variables for the motors to make code shorter to type
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2
#Set up timer
mytimer = 101
#Set up audio
wavfiles = ("r2d2-squeaks1.wav","r2d2-squeaks5.wav","r2d2-scream1.wav")
counter = 0
a = audioio.AudioOut(board.A0)
#Set up pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

def Compute_Theta(theta_in):
    ##Ok these are crickit frame signals
    x_total = 0
    y_total = 0
    z_total = 0
    for count in range(10):
        x, y, z = cpx.acceleration
        x_total = x_total + x
        y_total = y_total + y
        z_total = z_total + z
        time.sleep(0.001)
    x_total = x_total / 10
    y_total = y_total / 10
    z_total = z_total / 10
    
    #From here we need to rotate to the robot frame
    angle = 47.*math.pi/180.
    cos = math.cos(angle)
    sin = math.sin(angle)
    x_robot = cos*x_total + sin*y_total
    y_robot = -sin*x_total + cos*y_total
    
    #Then we can compute theta the pitch angle
    if abs(z_total) > 0.001:
        theta = math.atan(y_robot/z_total)*180./math.pi
    else:
        theta = 0.
    
    elapsed_time = 0.1
    
    thetadot = (theta_in - theta)/elapsed_time
    
    #print((x_robot,y_robot,z_total))
    return [theta,thetadot]

def Segway():
    #Initialize some Variables
    theta = 0
    thetadot = 0
    s = 0.8
    #Compute the angle of the robot
    [theta,thetadot_new] = Compute_Theta(theta)
    #Complimentary Filter
    thetadot = thetadot*s + thetadot_new*(1-s)

    #Drive the wheels based on the angle of the robot
    max_theta = 45. #Lowering this value increases the gain - make this larger to reduce oscillations
    kp = 1.0/(max_theta)
    kd = 0.5*kp
    drive = kp*theta + kd*thetadot
    if abs(drive) > 1.:
        drive = math.copysign(1.,drive)
    #motor_1.throttle = -drive
    #motor_2.throttle = -drive
    print((theta,thetadot))


def PLAY_SOUND(wavfile):
    print("Playing Sound",wavfile)
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    # You can now do all sorts of stuff here while the audio plays
    # such as move servos, motors, read sensors...
    # Or wait for the audio to finish playing:
    # While the music is playing run the light show
    while a.playing:
        Light_Show()
    f.close()
    return

def Light_Show():
    pixels.fill((255,255,255))
    pixels.show()
    Circle_Lights((0,255,0))
    Circle_Lights((255,0,0))
    Circle_Lights((0,0,255))
    End_Game()

def Circle_Lights(color):
    for idx in range(0,10):
        pixels[idx] = color
        pixels.show()
        time.sleep(0.1);
        pixels.fill((0,0,0))
        
def End_Game():
    for idx in range(0,4):
        pixels.fill((255,255,255))
        pixels.show()
        time.sleep(0.1)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.1)

while True:
    #I want to wait for a button press before I do anything
    if crickit.touch_1.value:
        #Play the Sound for the Robot
        PLAY_SOUND(wavfiles[counter])
        counter+=1
        if counter > 2:
            counter = 0
        #For 5 seconds I want to put the robot into "segway mode"
        mytimer = 0
        while mytimer < 100:
            Segway()
            mytimer+=1
        #Then I want to have the robot spin in circles
        mytimer = 0
        while mytimer < 100:
            motor_1.throttle = 1.
            motor_2.throttle = -1.