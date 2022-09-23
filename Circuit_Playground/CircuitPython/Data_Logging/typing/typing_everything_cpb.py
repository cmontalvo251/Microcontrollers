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
from adafruit_circuitplayground.bluefruit import cpb

#print(dir(cpx)) - Not supported in 6.1.0

# Set the keyboard object!
# Sleep for a bit to avoid a race condition on some systems
time.sleep(1)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is only current option...

print("Time\tLight\tTemperature\tX\tY\tZ")  # Print column headers

def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.02)   # use 1/5 second pause between characters

timestart = 0
timewindow = 100
while True:
    time.sleep(0.1)
    light = cpb.light
    temp = cpb.temperature
    sound_level = cpb.sound_level
    x,y,z = cpb.acceleration
    t = time.monotonic()
    print(t,x,y,z,light,temp,sound_level)
    if cpb.switch:
        cpb._led.value = True
        #x,y,z = cpx.acceleration
        # Format data into value 'output'
        #output = "%0.1f\t%0.1f\t%0.1f\t%0.5f\t%0.5f\t%0.5f" % (time.monotonic(), light,temp,x,y,z)
        output = "%0.5f\t%0.1f\t%0.1f\t%0.1f\t%0.1f\t%0.1f\t%0.1f" % (time.monotonic(),x,y,z,light,temp,sound_level)
        print(output)         # Print to serial monitor
        slow_write(output)    # Print to spreadsheet

        kbd.press(Keycode.DOWN_ARROW)  # Code to go to next row
        time.sleep(0.01)
        kbd.release_all()
        for _ in range(6):
            kbd.press(Keycode.LEFT_ARROW)
            time.sleep(0.005)
            kbd.release_all()
            #time.sleep(0.025)  # Wait a bit more for Google Sheets

        cpb._led.value = False
        # Change 0.1 to whatever time you need between readings
        time.sleep(1.0)