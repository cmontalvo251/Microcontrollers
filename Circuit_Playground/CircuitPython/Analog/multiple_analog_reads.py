import time
import board
from analogio import AnalogIn

pins = [board.A1,board.A2,board.A4,board.A6,board.A8]
analogin = []
for p in pins:
    analogin.append(AnalogIn(p))

def getVoltage(pin):  # helper
    return (pin.value * 3.3) / 65536

while True:
    output = ''
    for analog in analogin:
        val = getVoltage(analog)
        output += str(val)
        output += ' '
    print(output)
    time.sleep(0.1)