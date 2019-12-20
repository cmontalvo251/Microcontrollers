import time
import board
from analogio import AnalogIn
from adafruit_circuitplayground.express import cpx

#print(dir(cpx._light._photocell))
#print(dir(cpx))
#print(dir(board))

#time.sleep(10)

#analogin = AnalogIn(cpx._light.pin)

#def getVoltage(pin):  # helper
#    return (pin.value * 3.3) / 65536

while True:
    #print("Analog Voltage: %f" % getVoltage(analogin))
    #val = getVoltage(analogin)
    val = cpx._light._photocell.value
    print((val,))
    time.sleep(1)