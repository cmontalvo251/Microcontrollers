import board
import digitalio
import time
import pulseio
import adafruit_irremote
import neopixel
from digitalio import DigitalInOut, Direction, Pull
#import numpy as np ##This micro controller does not support numpy unfortunately

MAX_THRESHOLD = 1000
RANDOMCOLORS = False
buttonA = DigitalInOut(board.BUTTON_A)
buttonA.direction = Direction.INPUT
buttonA.pull = Pull.DOWN
buttonB = DigitalInOut(board.BUTTON_B)
buttonB.direction = Direction.INPUT
buttonB.pull = Pull.DOWN

def mean(input_signal):
    return sum(input_signal)/len(input_signal)

#Let's make the function to compute the RMS
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

##This will edit a signal to comply with a certain standard that I found
#in fuzzy_logic_IR_signals
def process_signal(input_signal):
    num_peaks = 0
    peak = 0
    for x in range(0,len(input_signal)):
        if input_signal[x] > 1500:
            input_signal[x] = 1500
            num_peaks += 1
            if peak == 0:
                #This means the previous value was not a peak so set it to 1
                peak = 1
            elif peak == 1:
                #this means previous value was a peak so we can't have repeated peaks
                input_signal[x] = 650
                peak = 0
        elif input_signal[x] < 650:
            peak = 0
            input_signal[x] = 650
        if input_signal[x] < 1200:
            peak = 0
            input_signal[x] = 650
    #We need to process the signal one more time and find the first data point 
    found = 0
    x = -1
    #that is about 1400
    while not found:
        x+=1
        if input_signal[x] > 1400:
            found = 1
    #Clip the first data points
    output_signal = input_signal[x-1:]
    #Turns out this clips everything to 49+ data points so let's clip everything to 49
    #Now clip the input_signal to only 49 data points
    if len(output_signal) > 49:
        n = len(output_signal) - 49
        del output_signal[-n:]
    return output_signal,num_peaks

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

#Right and up button averages
right_button_avg = [650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0]
up_button_avg = [650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 650.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0, 1500.0, 650.0]
#print(len(up_button_avg))


#Led for debug purposes
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

red = 0
blue = 0
green = 0

while True:
    if buttonA.value:
        RANDOMCOLORS = True
        print("Button A Pressed")
    if buttonB.value:
        print("Button B Pressed")
        RANDOMCOLORS = False
    if RANDOMCOLORS == True:
        #Reset color and brightness
        print((red,blue,green))
        red+=20 
        blue-=13
        green+=17
        if red > 255:
            red = 0
        if blue < 0:
            blue = 255
        if green > 255:
            green = 0
        pixels.fill((red,blue,green))
        pixels.show()
        time.sleep(0.1)
    else:
        print("Waiting for Signal")
        ##Wait for a signal or maybe just casually check. Not sure
        det = d.read_pulses(p)
        ##If we get a signal check to see what signal it is
        print("Signal Detected")
        if det is not None:
            #print(det)
            ##Better idea. Let's just remove these errorneous signals
            det = [x for x in det if x < 2500]
            #Then check length
            L = len(det)
            if L > 55:
                #print('Line is appropriate length')
                ##Now we need to make everything either a 1500 or 650 and count the numebr of peaks
                processed_det,num_peaks = process_signal(det)
                ##If we don't have 15 peaks we throw it out
                if int(num_peaks) == 15:
                    #print('Peaks is equal to 15')
                    #For some reason we need to count peaks again
                    num_peaks_final = sum(x>1200 for x in processed_det)
                    if int(num_peaks_final) == 15:
                        #print('Peaks is really 15')
                        #Now we have an appropriate signal to compute RMS
                        up_button_RMS = RMS(processed_det,up_button_avg)
                        #print(up_button_RMS)
                        if up_button_RMS < 100:
                            #print('This is clearly an Up button press')
                            change_brightness()
                        else:
                            #Let's check right button
                            right_button_RMS = RMS(processed_det,right_button_avg)
                            #print(right_button_RMS)
                            if right_button_RMS < 100:
                                #print('This is clearly a right button press')
                                change_color()
                    #else:
                        #print('Peaks apparently was not 15')
                #else:
                    #print('Peaks is not 15')
            #else:
                #print('Line is too short',L)
            #No matter what we want to Reset Pulses
            ResetPulses()
            #change_color() #only for debugging
     
        pixels.brightness = pixel_brightness
        pixels.fill(color)
        pixels.show()
