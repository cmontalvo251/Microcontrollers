import time
import board
import adafruit_hcsr04
import analogio

# This line creates the distance sensor as an object.
sonar1 = adafruit_hcsr04.HCSR04(trigger_pin=board.A6, echo_pin=board.A5, timeout=.1)
sonar2 = adafruit_hcsr04.HCSR04(trigger_pin=board.A3, echo_pin=board.A2, timeout=.1)


while True:
    time.sleep(0.01)
    try:
        d1 = sonar1.distance
    except:
        d1 = 100.0
    try:
        d2 = sonar2.distance
    except:
        d2 = 100.0
    if d1 < 10:
        time1 = time.monotonic()
        print('Detected First Sonar = ',time1)
    if d2 < 10:
        time2 = time.monotonic()
        print('Detected Time 2 = ',time2)
        delta_time = time2 - time1
        if delta_time > 0:
            speed = 1 / delta_time
            print('SPeed = ',speed)
            time.sleep(10)
    print(time.monotonic(),d1,d2)
