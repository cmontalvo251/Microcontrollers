import time
import board
import digitalio
import analogio
from adafruit_clue import clue

#print(dir(clue))
#print(clue.color)

# Turn off the NeoPixel
clue.pixel.fill(0)

# Motor setup
motor = digitalio.DigitalInOut(board.P2)
motor.direction = digitalio.Direction.OUTPUT

# Soil sense setup
analog = analogio.AnalogIn(board.P1)

def read_and_average(analog_in, times, wait):
    analog_sum = 0
    for _ in range(times):
        analog_sum += analog_in.value
        time.sleep(wait)
    return analog_sum / times

clue_display = clue.simple_text_display(title=" CLUE Plant", title_scale=1, text_scale=3)
clue_display.show()

while True:
    # Take 100 readings and average them
    analog_value = read_and_average(analog, 100, 0.01)
    # Calculate a percentage (analog_value ranges from 0 to 65535)
    percentage = analog_value / 65535 * 100
    # Display the percentage
    clue_display[0].text = "Soil: {} %".format(int(percentage))
    # Print the values to the serial console
    r,g,b,c = clue.color
    t = r + g + b + c
    clue_display[2].text = "Light: {}".format(int(t))
    clue_display[2].color = (255,255,255)
    print((analog_value, percentage,t))

    if t < 1000:
        #motor.value = True
        clue_display[1].text = "Motor ON"
        clue_display[1].color = (0, 255, 0)
        time.sleep(0.5)

    # always turn off quickly
    motor.value = False
    clue_display[1].text = "Motor OFF"
    clue_display[1].color = (255, 0, 0)

