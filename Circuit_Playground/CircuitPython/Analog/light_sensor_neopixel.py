import time
import board
from analogio import AnalogIn
import neopixel

pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=1.0)

analogin = AnalogIn(board.A8)

def getVoltage(pin):  # helper
    return (pin.value * 3.3) / 65536

while True:
    #print("Analog Voltage: %f" % getVoltage(analogin))
    val = getVoltage(analogin)
    print((val,))
    if val > 2.5:
        pixels.fill((255,0,0))
    else:
        pixels.fill((0,0,0))
    time.sleep(0.1)
