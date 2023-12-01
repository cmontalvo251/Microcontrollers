# Write your code here :-)
# Circuit Playground Express Data Time/Light Intensity/Temp
# Log data to a spreadsheet on-screen
# Open Spreadsheet beforehand and position to start (A,1)
# Use slide switch to start and stop sensor readings
# Time values are seconds since board powered on (relative time)

import time
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
#https://learn.adafruit.com/circuitpython-essentials/circuitpython-hid-keyboard-and-mouse
import usb_hid
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_circuitplayground.express import cpx

#print(dir(cpx)) - Not supported in 6.1.0

# Set the keyboard object!
# Sleep for a bit to avoid a race condition on some systems
time.sleep(1)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is only current option...

print("Time\tTemperature")  # Print column headers

def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.02)   # use 1/5 second pause between characters
TOGGLE = False
print(dir(cpx))
while True:
    currenttime = time.monotonic()
    temp = cpx.temperature
    print(currenttime,temp)
    if cpx.button_a == True:
        TOGGLE = not TOGGLE
        time.sleep(1.0)
    if TOGGLE == True:
        output = "%0.1f\t%0.1f\n" % (time.monotonic(),temp)
        print(output)         # Print to serial monitor
        slow_write(output)    # Print to spreadsheet
        cpx._led.value = False
        # Change 0.1 to whatever time you need between readings
        time.sleep(0.1)
