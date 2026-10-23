import time
import board
import digitalio
import storage
import neopixel
import time
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
#https://learn.adafruit.com/circuitpython-essentials/circuitpython-hid-keyboard-and-mouse
import usb_hid
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS

##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

switch = digitalio.DigitalInOut(board.D7)  # For Circuit Playground Express
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

# --- SETUP PINS & INDICATORS ---
DT = digitalio.DigitalInOut(board.A4)
SCK = digitalio.DigitalInOut(board.A5)
DT.direction = digitalio.Direction.INPUT
SCK.direction = digitalio.Direction.OUTPUT
SCK.value = False

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

# Circuit Playground Express has 10 built-in pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.25)
colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]
c = 0

# --- HX711 FUNCTIONS ---
def read_hx711():
    while DT.value:
        pass
    count = 0
    for _ in range(24):
        SCK.value = True
        count = count << 1
        SCK.value = False
        if DT.value:
            count += 1
    SCK.value = True
    SCK.value = False
    if count & 0x800000:
        count -= 0x1000000
    return count

def read_average(samples=10):
    total = 0
    for _ in range(samples):
        total += read_hx711()
    return total / samples

def tare(samples=20):
    print("Calculating offset... Remove all weight.")
    total = 0
    for _ in range(samples):
        total += read_hx711()
    return total / samples

# --- MAIN SETUP ---
print("Remove all weight...")
time.sleep(3)
offset = tare()
print("Offset:", offset)
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)  # US is only current option...
def slow_write(string):   # Typing should not be too fast for
    for c in string:      # the computer to be able to accept
        layout.write(c)
        time.sleep(0.01)  
TOGGLE = False

scale = 213/80000.

# --- MAIN LOOP ---
startTime = time.monotonic()
while True:
    reading = read_average(1)
    weight = -(reading - offset) * scale
    t = time.monotonic()-startTime

    # Flash board LED to show activity
    led.value = not led.value

    if switch.value == False:
        TOGGLE = True
    else:
        TOGGLE = False

    # Check toggle state and log data
    if TOGGLE:
        print("Logging Data:", t, weight)
        output = "%0.1f\t%0.5f\n" % (t,weight)
        slow_write(output)    # Print to spreadsheet

        # Cycle pixel color on Pixel 0
        pixels[0] = colors[c]
        c = (c + 1) % len(colors)
    else:
        print("Not logging Data:",t,weight)
        pixels[0] = (0, 0, 0)

    time.sleep(0.01)
