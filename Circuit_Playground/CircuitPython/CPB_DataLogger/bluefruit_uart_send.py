# CircuitPython Bluefruit LE Connect Plotter Example

###Import Stuff
import board
import time
import analogio
import adafruit_thermistor
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
import adafruit_lis3dh
import busio
import digitalio
import math

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

##Setup thermistor
thermistor = adafruit_thermistor.Thermistor(board.TEMPERATURE, 10000, 10000, 25, 3950)
print('Thermistor and Light Sensor Setup')

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G
print('Accelerometer Setup')

while True:
    # Advertise when not connected.
    print('Not connected')
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
        t = time.monotonic()
        #Print thermistor to serial
        T = thermistor.temperature
        #Accelerometer
        x,y,z = lis3dh.acceleration

        #Print to STDOUT
        print((t,x,y,z,T))

        #And send them over uart (which is basically serial) but this is _server
        #which means it's wireless
        #uart_server.write('{},{}\n'.format(scale(l), t))
        uart_server.write('{},{},{}\n'.format(x,y,z))
        #uart_server.write('{}\n'.format(theta))

        #Sleep for 5Hz
        time.sleep(0.2)# Write your code here :-)
