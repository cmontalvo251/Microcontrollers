# CircuitPython NeoPixel Color Picker Example

import board
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.color_packet import ColorPacket
import digitalio
import time

##Setup up BlueTooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)

##Setup Pixels on board
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.1)

##Setup pin 13
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT


while True:
    # Advertise when not connected.
    print('Advertising')
    ble.start_advertising(advertisement)
    ##Just keep looping if you're not connected
    while not ble.connected:
        pass
    print('Ok connected!')
    ble.stop_advertising()

    #Once connected loop through here
    while ble.connected:
        ##If you get a packet check out its contents
        packet = Packet.from_stream(uart_server)
        led.value = True
        ##If it's a ColorPacket
        if isinstance(packet, ColorPacket):
            print(packet.color)
            ##Fill all the pixels with that color
            pixels.fill(packet.color)
            for i in range(0,5):
                led.value = False
                time.sleep(0.1)
                led.value = True
                time.sleep(0.1)