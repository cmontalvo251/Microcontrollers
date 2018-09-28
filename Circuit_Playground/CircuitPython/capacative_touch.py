import time
from adafruit_crickit import crickit
 
# Capacitive touch tests
 
while True:
    time.sleep(0.1)
    #print("Checking Pad ")
    print((crickit.touch_1.raw_value,crickit.touch_1.value))    #open up the plotter and you can see the raw data
    if crickit.touch_1.value:
        print("Touched Cap Touch Pad 1")
        #time.sleep(1)