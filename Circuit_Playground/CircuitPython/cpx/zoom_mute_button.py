from digitalio import DigitalInOut, Direction, Pull
import board
import time
import digitalio
import usb_hid
from adafruit_circuitplayground.express import cpx
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

kbd = Keyboard(usb_hid.devices)

def key_press():
    print('Sending oh shit to Zoom')
    #kbd.send(Keycode.ALT,Keycode.CONTROL,Keycode.SHIFT)
    #kbd.send(Keycode.ALT,Keycode.A)

CURRENT_STATE = False

while True:
    print(time.monotonic(),int(cpx.button_a))
    if int(cpx.button_a) == 1:
        if CURRENT_STATE == False:
            print('We have just pressed the button')
            key_press()
            CURRENT_STATE = True
        print('Muted')
        cpx.red_led = True
        time.sleep(.5)
    elif int(cpx.button_a) == 0:
        if CURRENT_STATE == True:
            print('We have just let go of the button')
            key_press()
            CURRENT_STATE = False
        cpx.red_led = True
        time.sleep(0.5)
        print('Unmuted')
        cpx.red_led = False
        time.sleep(0.5)
        print('Unmuted')
        cpx.red_led = True
        time.sleep(0.5)
        print('Unmuted')
        cpx.red_led = False
        time.sleep(0.5)
        print('Unmuted')