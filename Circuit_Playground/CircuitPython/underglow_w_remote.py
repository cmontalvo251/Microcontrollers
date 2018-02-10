import board
import digitalio
import time
import pulseio
import adafruit_irremote
import neopixel
#import numpy as np ##This micro controller does not support numpy unfortunately

MAX_THRESHOLD = 1000

def mean(input_signal):
    return sum(input_signal)/len(input_signal)
    
def mean_matrix(input_matrix):
    output_matrix = []
    r = len(up_button)
    c = 67 #This is a horrible hack but it will work
    for ci in range(0,c):
        avg = 0
        for ri in range(0,r):
            avg+= input_matrix[ri][ci]
        avg /= r
        output_matrix.append(avg)
    return output_matrix

##Let's make the function to compute the RMS
def RMS(input_signal,avg_signal):
    global MAX_THRESHOLD
    ##Assuming both signals are the same and np arrays we should be able to do this
    ##But they might not
    if len(input_signal) != len(avg_signal):
        return MAX_THRESHOLD*100.0
    else:
        avg = mean(avg_signal)
        num = 0
        for x in range(0,len(input_signal)):
            num+=(input_signal[x]-avg_signal[x])**2
        return num/avg

##Up Button Press
def change_brightness():
    global pixel_brightness
    pixel_brightness+=0.05
    if pixel_brightness > 0.25:
        pixel_brightness = 0.0

##Right Button Press
def change_color():
    global COLORMODE,color
    COLORMODE+=1
    if COLORMODE == 0:
        color = (255,255,255)
    elif COLORMODE == 1:
        color = (255,0,0)
    elif COLORMODE == 2:
        color = (0,255,0)
    elif COLORMODE == 3:
        color = (0,0,255)
        COLORMODE = -1
    time.sleep(0.2)
    
def ResetPulses():
    global p
    p.clear()
    p.resume()


right_button_avg = [650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0]

up_button_avg = [650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 650.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0,
 1500.0,
 650.0]
                    
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

#Set up pixels
pixel_brightness = 0.15
color = (255,255,255)
COLORMODE = 0
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)
pixels.fill(color)
pixels.show()

p = pulseio.PulseIn(board.REMOTEIN,maxlen=120,idle_state=True)
d = adafruit_irremote.GenericDecode()
ResetPulses()
while True:
    ##Wait for a signal or maybe just casually check. Not sure
    det = d.read_pulses(p)
    ##If we get a signal check to see what signal it is
    if det is not None:
        up = RMS(det,up_button_avg)
        right = RMS(det,right_button_avg)
        if up < MAX_THRESHOLD:
             ##This is a up button press
             change_brightness()
        elif right < MAX_THRESHOLD:
             ##This is a right button press
            change_color()
        #No matter what we want to Reset Pulses
        ResetPulses()
        print(det)
        print(up) #only for debugging
        print(right)
        #change_color() #only for debugging
     
    #Reset color and brightness
    pixels.brightness = pixel_brightness
    pixels.fill(color)
    pixels.show()
