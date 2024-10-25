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
import digitalio
import board
from analogio import AnalogIn

#print(dir(cpx)) - Not supported in 6.1.0

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

# Set the keyboard object!
# Sleep for a bit to avoid a race condition on some systems
time.sleep(1)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is only current option...

def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.02)   # use 1/5 second pause between characters
TOGGLE = False
analogin = AnalogIn(board.A6)
while True:
    currenttime = time.monotonic()
    print(currenttime,analogin.value,TOGGLE)
    if buttonA.value == True:
        TOGGLE = not TOGGLE
        print(TOGGLE)
        time.sleep(1.0)
    if TOGGLE == True:
        output = "%0.1f\t%0.1f\n" % (time.monotonic(),analogin.value)
        print(output)         # Print to serial monitor
        slow_write(output)    # Print to spreadsheet
        # Change 0.1 to whatever time you need between readings
    time.sleep(0.1)
