##Import all the modules I need
import analogio
import time
import board

##Setup the right elbow as an analog signal input
right_elbow = analogio.AnalogIn(board.A2)

##Setup the left elbow as an analog signal input
left_elbow = analogio.AnalogIn(board.A4)

##Setup the lights

##Loop forever
while True:
    ##Read the analog signal of the left elbow (0-65536)
    left_signal = left_elbow.value

    ##Read the analog signal of the right elbow (0-65536)
    right_signal = right_elbow.value

    ##DEBUG
    print(left_signal,right_signal)
    #print(left_signal)
    time.sleep(0.1)

    ##determine if push up has been acheieved
    ##when right and left elbow are touching the analog signal is around 40000
    ##when they are not touching the signal is about 600.
    #if the left elbow and right elbow are well above 600 around 40000. then a pushup has been successful.
    if left_signal > 10000 and right_signal > 10000:
        print('PUSH UP ACHIEVED!!!')