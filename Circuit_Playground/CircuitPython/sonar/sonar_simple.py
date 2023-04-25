import time
import board
import adafruit_hcsr04
import analogio

# This line creates the distance sensor as an object.
sonar = adafruit_hcsr04.HCSR04(trigger_pin=board.A2, echo_pin=board.A1, timeout=.1)

while True:
    time.sleep(0.1)

    try:
        d = sonar.distance
    except:
        d = 100.0
    print(time.monotonic(),d)
