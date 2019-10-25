import board
import digitalio
import time
from analogio import AnalogIn,AnalogOut

##Big LEDs
#big_led = digitalio.DigitalInOut(board.D3)
#big_led.direction = digitalio.Direction.OUTPUT

#Rear Led
#led = digitalio.DigitalInOut(board.D13)
#led.direction = digitalio.Direction.OUTPUT

#Breadboard LED
#led_board = digitalio.DigitalInOut(board.D4)
#led_board.direction = digitalio.Direction.OUTPUT
#led_board.value = 0.0

led_analog = AnalogOut(board.A0)

analog_button = AnalogIn(board.A4)

#switch = digitalio.DigitalInOut(board.D4)
#switch.direction = digitalio.Direction.INPUT
#switch.pull = digitalio.Pull.UP

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.A1)
threshold = 15.0
#voltage = 2.1
while True:
    #voltage += 0.1
    #if voltage > 5.0:
    #    voltage = 2.1
    #value = int(voltage * 65535 / 5.0)
    #led_analog.value = analog_button.value
    #print((voltage,value))
    #print((switch.value,))
    #print((analog_button.value*5.0/65535,))
    time.sleep(0.1)
    light = photocell.value*330/(2**16)
    print((light,))
    #if light < threshold:
    #    big_led.value = True
    #    led_board.value = False
    led_analog.value = int(5.0 * 65535 / 5.0)
    #else:
        #led_analog.value = 0
    #else:
    #    big_led.value = False
    #    led_board.value = True
            
    