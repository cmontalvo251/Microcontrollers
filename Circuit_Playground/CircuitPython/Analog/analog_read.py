import time
import board
from analogio import AnalogIn

analogin = AnalogIn(board.A8)

def getVoltage(pin):  # helper
    return (pin.value * 3.3) / 65536

bootuptime = time.monotonic()
while True:
    time_elapsed = time.monotonic() - bootuptime
    #print("Analog Voltage: %f" % getVoltage(analogin))
    val = getVoltage(analogin)
    print((val,))
    time.sleep(0.1)