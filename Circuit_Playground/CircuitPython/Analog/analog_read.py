import time
import board
from analogio import AnalogIn

analogin = AnalogIn(board.A6)

while True:
    print((analogin.value,))
    time.sleep(0.1)
