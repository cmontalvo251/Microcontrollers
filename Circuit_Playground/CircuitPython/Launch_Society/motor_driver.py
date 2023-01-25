import board
import digitalio
import time
from analogio import AnalogOut
import neopixel

##Setup Pin 10(Arduino) A0(CPX) - Motor driver pin
motorout = AnalogOut(board.A0)

##Create Neopixel Object for UI
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.2, auto_write=True)

##Setup Pins 6 and 7 (Arduino) A2/3 (CPX) - Motor Direction Pins
pin1 = digitalio.DigitalInOut(board.A2)
pin1.direction = digitalio.Direction.OUTPUT
pin2 = digitalio.DigitalInOut(board.A3)
pin2.direction = digitalio.Direction.OUTPUT
altitude = 0.0
MOTORDONE = False
while True:
    ##Measure Pressure sensor
    altitude = altitude + 1

    print('Time = ',time.monotonic(),'Altitude = ',altitude)

    if altitude > 10 and MOTORDONE == False:
        print(time.monotonic(),'Moving Motor')
        pin1.value = True
        pin2.value = False
        motorout.value = 65535 #16 bit instead of 8 bit on the Arduino
        pixels.fill((255,0,0))
        time.sleep(5.0)
        motorout.value = 0
        pixels.fill((0,0,0))
        MOTORDONE = True
    else:
        motorout.value = 0
    time.sleep(1.0)
