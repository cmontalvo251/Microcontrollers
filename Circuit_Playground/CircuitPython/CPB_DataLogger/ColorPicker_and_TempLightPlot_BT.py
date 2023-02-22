###Import time,board,analogio and adafruit_thermistor for temperature
import time
import board
import analogio
import adafruit_thermistor

##Import Adafruit BLE
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

###Import items to get packets and color packets from bluetooth
from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.color_packet import ColorPacket

##Setup BLE
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)

###Setup thermistor
thermistor = adafruit_thermistor.Thermistor(board.TEMPERATURE, 10000, 10000, 25, 3950)
##Setup light sensor as well
light = analogio.AnalogIn(board.LIGHT)

def scale(value):
    return value / 65535 * 50

####Import neopixel for fun
from rainbowio import colorwheel
import neopixel
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.2, auto_write=False)
RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
WHITE = (255, 255, 255)
OFF = (0, 0, 0)
color = GREEN #default color

color_chase_demo = 1
def color_chase(color, wait):
    for i in range(10):
        pixels[i] = color
        time.sleep(wait)
        pixels.show()
    time.sleep(0.5)

###Infinite while loop
while True:
    # Advertise when not connected.
    ble.start_advertising(advertisement)
    ##Do the color_chase function when ble is not connected
    while not ble.connected:
        color_chase(BLUE, 0.1)
        color_chase(OFF, 0.1)
        time.sleep(.1)
    ble.stop_advertising()

    ##Once you're connected
    while ble.connected:
        ##Get the temperature
        temp = thermistor.temperature*9/5 + 32

        ##If temp is between 60 and 80 fill the pixels with the color from the color picker
        if 60 <= temp <= 80 :
            pixels.fill(color)
            pixels.show()
        #If it's less than 60 different color
        if temp < 60:
            pixels.fill((0, 0, 150))
            pixels.show()
        ##Greater than 80 different color
        if temp > 80:
            pixels.fill((150, 0, 0))
            pixels.show()
        ###Print the light and temperature to the REPL
        print((scale(light.value), temp))
        ###Send the temp and light values over ble
        uart_server.write("{},{}\n".format(scale(light.value), temp))
        time.sleep(0.5)

        ##check to see if there is a message.
        if uart_server.in_waiting:
            ##Save the message as a packet
            packet = Packet.from_stream(uart_server)
            ##If the message / packet is a colorpacket
            if isinstance(packet, ColorPacket):
                ##Print the color
                print(packet.color)
                ##and save it to a color variable
                color = packet.color