###ORIGINALLY SUBMITTED BY MR. POPE IN SPRING OF 2020#####
###I DO NOT CLAIM OWNERSHIP OF THIS SOFTWARE AND ALL RIGHTS ARE
##RESERVED FOR MR. POPE SPG 2020#############3

#importing required libraries
import curses
import os
import time
import picamera
import RPi.GPIO as GPIO
from datetime import datetime
import pytz

print('up arrow: tilt up')
print('down arrow: tilt down')
print('left arrow: pan left')
print('right arrow: pan right')
print('p: take photo')
print('v: take video')
print('s: stop recording')
print('q: quit program')

camera = picamera.PiCamera()
camera.resolution = (1024, 768)
camera.start_preview()

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(15,GPIO.OUT)
GPIO.output(15,GPIO.LOW)

Central = pytz.timezone("US/Central")
now = datetime.now(Central)
hernow=now.strftime("%m-%d-%Y_%H:%M:%S")

#activating servo blaster (servod must be in the same folder as this script!)
os.system('sudo ./servod')

#flipping the camera for so its not upside down
camera.vflip = True
camera.hflip = True

# get the curses screen window
screen = curses.initscr()

# turn off input echoing
curses.noecho()

# respond to keys immediately (don't wait for enter)
curses.cbreak()

# map arrow keys to special values
screen.keypad(True)

#setting start up serrvo positions
#positions range from (50-250)
servo1 = 100
servo2 = 100
# print doesn't work with curses, use addstr instead
#pic = 1
#vid = 1
try:
    while True:
        char = screen.getch()
        if char == ord('q'):
            #if q is pressed quit
            break
        if char == ord('p'):
            #if p is pressed take a photo!
            camera.capture('/home/pi/Pictures/Snapshot%s.jpg' %hernow)
            #pic = pic +1
            screen.addstr(0, 0, 'Snapshot Taken! ')
            
            count = 0
            while count < 2:
                GPIO.output(15,GPIO.HIGH)
                time.sleep (0.1)
                GPIO.output(15,GPIO.LOW)
                time.sleep (0.1)
                count = count + 1
        elif char == ord('v'):
            camera.start_recording('/home/pi/Videos/Recording%s.h264' %hernow)
            #vid = vid +1
            screen.addstr(0,0, 'Recording.')
            count = 0
            while count < 4:
                GPIO.output(15,GPIO.HIGH)
                time.sleep (0.1)
                GPIO.output(15,GPIO.LOW)
                time.sleep (0.1)
                count = count + 1
        elif char == ord('s'):
            camera.stop_recording()
            screen.addstr(0,0,'Recording Stopped.')
            
            GPIO.output(15,GPIO.LOW)
        elif char == curses.KEY_RIGHT:
            screen.addstr(0, 0, 'right ')
            if servo1 > 50:
                servo1 = servo1 -2
            os.system("echo 0=%s > /dev/servoblaster" %servo1) 
            time.sleep(0.005)
        elif char == curses.KEY_LEFT:
            screen.addstr(0, 0, 'left ')
            if servo1 < 150:
                servo1 = servo1 +2
            os.system("echo 0=%s > /dev/servoblaster" %servo1) 
            time.sleep(0.005)
        elif char == curses.KEY_UP:
            screen.addstr(0, 0, 'up ')
            if servo2 < 150:
                servo2 = servo2 +2
            os.system("echo 1=%s > /dev/servoblaster" %servo2) 
            time.sleep(0.005)
        elif char == curses.KEY_DOWN:
            screen.addstr(0, 0, 'down ')
            if servo2 > 50:    
                servo2 = servo2 -2
            os.system("echo 1=%s > /dev/servoblaster" %servo2) 
            time.sleep(0.005)
finally:
    # shut down cleanly
    curses.nocbreak(); screen.keypad(0); curses.echo()
    curses.endwin()
#######################REF#########################
#https://github.com/richardghirst/PiBits/tree/master/ServoBlaster
#https://www.youtube.com/redirect?redir_token=hZeL5TcmwtW81ZeT3ZSJhNq8pPN8MTU4ODcyMjg1MUAxNTg4NjM2NDUx&q=https%3A%2F%2Fgithub.com%2Ftuckershannon%2FPanTilt%2Farchive%2Fmaster.zip&event=video_description&v=wj2CmN_dLQw
