from adafruit_crickit import crickit
import audioio
import time
import board
import neopixel

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
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.25)

def PLAY_SOUND(wavfile):
    print("Playing Sound",wavfile)
    f = open(wavfile, "rb")
    wav = audioio.WaveFile(f)
    a.play(wav)
    # You can now do all sorts of stuff here while the audio plays
    # such as move servos, motors, read sensors...
    # Or wait for the audio to finish playing:
    Light_Show()
    # While the music is playing run the light show
    while a.playing:
        pass
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
        time.sleep(0.01);
        pixels.fill((0,0,0))
        
def End_Game():
    for idx in range(0,4):
        pixels.fill((255,255,255))
        pixels.show() 
        time.sleep(0.01)
        pixels.fill((0,0,0))
        pixels.show()
        time.sleep(0.01)

pixels.fill((255,255,255))

time.sleep(2)

pixels.fill((255,0,0))

N = 1000
initial_cap = 0.0
for i in range(0,N):
    initial_cap += crickit.touch_1.raw_value
initial_cap /= N

print(initial_cap)
time.sleep(1)

pixels.fill((0,255,0))

while True:
    #I want to wait for a button press before I do anything
    #print("Waiting for button Press")
    percent_change = 100*abs(crickit.touch_1.raw_value-initial_cap)/initial_cap
    print(percent_change)
    if percent_change > 15:
        #Play the Sound for the Robot
        PLAY_SOUND(wavfiles[counter])
        counter+=1
        if counter > 2:
            counter = 0
        time.sleep(0.1)
        #Then I want to have the robot spin in circles
        mytimer = 0
        while mytimer < 20:
            motor_1.throttle = 1.0
            motor_2.throttle = -1.0
            time.sleep(0.1)
            mytimer+=1
            print(mytimer)
        motor_1.throttle = 0.
        motor_2.throttle = 0.
