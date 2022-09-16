import time
import board
import busio
import digitalio
import adafruit_lis3dh
import math
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

##Button Presses
buttonS = digitalio.DigitalInOut(board.SLIDE_SWITCH)
buttonS.direction = digitalio.Direction.INPUT
buttonS.pull = digitalio.Pull.UP

# Set the keyboard object!
# Sleep for a bit to avoid a race condition on some systems
time.sleep(1)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is only current option...

def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.02)   # use 1/5 second pause between characters

while True:
    x,y,z = lis3dh.acceleration
    print(time.monotonic(),x,y,z,buttonS.value)
    time.sleep(0.1)
    if buttonS.value == True:
        print("Logging Data")
        # Format data into value 'output'
        output = "%0.2f\t%0.5f\t%0.5f\t%0.5f" % (time.monotonic(),x,y,z)
        print(output)         # Print to serial monitor
        slow_write(output)    # Print to spreadsheet

        kbd.press(Keycode.DOWN_ARROW)  # Code to go to next row
        time.sleep(0.005)
        kbd.release_all()
        for _ in range(3):
            kbd.press(Keycode.LEFT_ARROW)
            time.sleep(0.005)
            kbd.release_all()