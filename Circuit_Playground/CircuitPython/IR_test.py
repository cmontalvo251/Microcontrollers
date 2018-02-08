import board
import digitalio
import time
import pulseio
import adafruit_irremote
import neopixel

##B Button Press
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
p.clear()
p.resume()
while True:
    det = d.read_pulses(p)
    if det is not None:
        print(det)
        change_color()
        p.clear()
        p.resume()
    #Reset color and brightness
    pixels.brightness = pixel_brightness
    pixels.fill(color)
    pixels.show()
