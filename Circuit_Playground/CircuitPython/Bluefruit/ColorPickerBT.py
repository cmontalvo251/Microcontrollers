###IMport board, neopixel and time
import board
import neopixel
import time

###Import items to get packets and color packets from bluetooth
from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.color_packet import ColorPacket

###Standard Bluetooth module
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

###Set up bluetooth
ble = BLERadio()
uart_service = UARTService()
advertisement = ProvideServicesAdvertisement(uart_service)

##Setup Pixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.1)

##Inifite while loop
while True:
    ##Start Advertising
    ble.start_advertising(advertisement)
    ##Loop until you're connected
    while not ble.connected:
        print(time.monotonic())
        time.sleep(0.3)
        pass
    #Once connected stop advertising
    ble.stop_advertising()

    ##While connected
    while ble.connected:
        ##check to see if there is a message.
        if uart_service.in_waiting:
            ##Save the message as a packet
            packet = Packet.from_stream(uart_service)
            ##If the message / packet is a colorpacket
            if isinstance(packet, ColorPacket):
                ##Print the color
                print(packet.color)
                ##And then set the color of the neopixels
                pixels.fill(packet.color)