# Circuit Playground Express Data Logger
# Log data to a spreadsheet on-screen
# Open Spreadsheet beforehand and position to start (A,1)
# Use slide switch to start and stop sensor readings
# Time values are seconds since board powered on (relative time)
# Adapted from Adafruit Forums

import time
import digitalio
import board
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

# Set the keyboard object!
# Sleep for a bit to avoid a race condition on some systems
time.sleep(1)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is currently only option.

print("Time\tButton Value")  # Print column headers

def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.02)   # use 1/5 second pause between characters

while True:
    if switch.value == True:
        # Turn on the LED to show we're logging
        output = "%0.1f\t%d" % (time.monotonic(), int(buttonA.value))
        print(output)         # Print to serial monitor
        slow_write(output)    # Print to spreadsheet

        kbd.press(Keycode.DOWN_ARROW)  # Code to go to next row
        time.sleep(0.01)
        kbd.release_all()
        for _ in range(2):
            kbd.press(Keycode.LEFT_ARROW)
            time.sleep(0.005)
            kbd.release_all()
            #time.sleep(0.025)  # Wait a bit more for Google Sheets
    else:
        print('Not logging. Flip Switch to start logging')
    # Change 0.1 to whatever time you need between readings
    time.sleep(1.0)