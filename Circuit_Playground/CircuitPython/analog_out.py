import time
from analogio import AnalogOut
import board
analogout = AnalogOut(board.A0)
value = 0
while True:
    #value += 1000
    #if value > 65535:
    #    value = 0
    #analogout.value = value
    analogout.value = 65535
    #print(value*5.0/65535)
    time.sleep(0.1)