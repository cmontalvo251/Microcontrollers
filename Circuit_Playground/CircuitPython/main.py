import time
import board
from analogio import AnalogIn
import adafruit_thermistor
import busio
import digitalio
import adafruit_lis3dh

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.A8)

while True:
    #print("Analog Voltage: %f" % getVoltage(analogin))
    light = photocell.value*330/(2**16)
    print("Time (sec) = ",time.monotonic(),"Light (Lux) =",light
    time.sleep(0.5)