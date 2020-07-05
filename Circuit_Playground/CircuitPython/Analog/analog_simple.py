import time
import board
import analogio

analog = analogio.AnalogIn(board.A2)

while True:
    print(time.monotonic(),analog.value)
    time.sleep(0.05)