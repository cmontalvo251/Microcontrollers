# CircuitPython Bluefruit LE Connect Button Send Example
###Import Stuff
import board
import time
import analogio
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
import busio
import digitalio
import math

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

##SETUP Button
##Button Presses
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN
print('Button Setup')
start_time = time.monotonic()
while True:
    # Advertise when not connected.
    print('Not connected')
    print('Look for',ble.name)
    ble.start_advertising(advertisement)
    ##Keep looping until connection established
    while not ble.connected:
        pass
    #Stop advertising once connected
    print('Connected')
    ble.stop_advertising()

    ##Once connected
    while ble.connected:
        #Time =
        t = time.monotonic()-start_time
        #Button press
        b = int(buttonA.value)

        #Print to STDOUT
        print((t,b))

        #And send them over uart (which is basically serial) but this is _server
        uart_server.write('{},{}\n'.format(t,b))

        #Sleep for 5Hz
        time.sleep(0.2)# Write your code here :-)