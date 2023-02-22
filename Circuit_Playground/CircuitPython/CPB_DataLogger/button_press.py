# CircuitPython NeoPixel Color Picker Example

import board
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

###Types of Packets
from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.color_packet import ColorPacket
from adafruit_bluefruit_connect.button_packet import ButtonPacket

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

# Advertise when not connected.
def advertise():
    print('Advertising')
    ble.start_advertising(advertisement)
    return True

def stop_advertising():
    ble.stop_advertising()
    return False

def blink():
    for i in range(0,5):
        led.value = False
        time.sleep(0.1)
        led.value = True
        time.sleep(0.1)

advertising = advertise()

while True:
    if not ble.connected:
        ##This means we aren't connected
        #Are we advertising?
        if not advertising:
            ##Then advertise
            advertising = advertise()
    else:
        ##This means we are connected
        print('Ok connected!')
        ##Are we advertising
        if advertising:
            advertising = stop_advertising()

        ##If we are connected we need to constantly check for packets.
        ##If you get a packet check out its contents
        packet = Packet.from_stream(uart_server)
        if packet is not None:
            print(packet)
            blink()
        led.value = True
        ##If it's a ColorPacket
        if isinstance(packet, ColorPacket):
            #print(packet.color)
            ##Fill all the pixels with that color
            pixels.fill(packet.color)
        elif isinstance(packet, ButtonPacket) and packet.pressed:
            if packet.button == ButtonPacket.BUTTON_1:
                print('Button 1 Pressed')
            elif packet.button == ButtonPacket.BUTTON_2:
                print('Button 2 Pressed')

