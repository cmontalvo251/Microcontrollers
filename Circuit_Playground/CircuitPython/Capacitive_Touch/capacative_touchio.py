import time
import board
import touchio

touch = touchio.TouchIn(board.A2)

while True:
    print(time.monotonic(),touch.raw_value)
    time.sleep(0.5)