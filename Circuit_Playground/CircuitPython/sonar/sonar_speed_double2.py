import time
import board
from adafruit_circuitplayground.express import cpx
import neopixel
import adafruit_hcsr04

# Ultrasonic sensor setup
sonar1 = adafruit_hcsr04.HCSR04(trigger_pin=board.A2, echo_pin=board.A3, timeout=.1)
sonar2 = adafruit_hcsr04.HCSR04(trigger_pin=board.A4,echo_pin=board.A5,timeout=.1)


# NeoPixel setup
# Number of pixels
num_pixels = 145
# Define the pin where the NeoPixels are connected
pixel_pin = board.A1
# Create a NeoPixel object
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=0.2, auto_write=False)
# Function to wheel color

while True:
    time.sleep(0.01)  # how fast data is being taken
    try:
        d1 = sonar1.distance
    except:
        d1 = 100.0
    try:
        d2 = sonar2.distance
    except:
        d2 = 100.0
    if d1 < 20 and d1 > 1.0:
        time1 = time.monotonic()
        print('Detected Time 1 = ', time1)
    if d2 < 20 and d2 > 1.0:
        time2 = time.monotonic()
        print('Detected Time 2 = ', time2)
        delta_time = time2 - time1
        if delta_time > 0:
            speed = 1 / delta_time
            print('Speed = ', speed)
            # Trigger lights and speaker here
            cpx.play_tone(440, 1)  # Play a tone
            for _ in range(3):  # Flash NeoPixels 3 times
                pixels.fill((255, 0, 0))
                pixels.show()
                time.sleep(0.5)
                pixels.fill((0, 0, 0))
                pixels.show()
                time.sleep(0.5)
            time.sleep(5)  # Wait for 5 seconds before checking again    
    print(time.monotonic(), d1,d2)
