import time
import board
import busio
import pulseio
import neopixel
from adafruit_crickit import crickit
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

print("This is SPARTA!!")

ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)

ss = crickit.seesaw

crickit.drive_1.frequency = 1000

fsr = crickit.SIGNAL2

num_pixels = 24 # number of pixels being driven
pixels = neopixel.NeoPixel(board.A1, num_pixels, brightness=0.3, auto_write=False)

##Button Presses
BUTTON_1 = crickit.SIGNAL1
ss.pin_mode(BUTTON_1, ss.INPUT_PULLUP)

print('So far so good')

ADVERTISING = False

while True:

    t = time.monotonic()
    fsr_val = ss.analog_read(fsr)

    if not ble.connected:
        if ADVERTISING == False:
            ble.start_advertising(advertisement)
            ADVERTISING = True
        else:
            print('Advertising, Look for',ble.name)
    else:
        ble.stop_advertising()
        ADVERTISING = False
        uart_server.write('{},{}\n'.format(t,fsr_val))

    if ss.digital_read(BUTTON_1) == False:
        print('Detach')
        crickit.drive_1.fraction = 0.0
        print("Let there be Light!!!")
        pixels.fill((255,0,0))
    else:
        pixels.fill((0,0,0))
        crickit.drive_1.fraction = 1.0
    if fsr_val > 200:
        print('HIT!!')
        pixels.fill((255,65,0))

    pixels.show()
    print((t,fsr_val))
    time.sleep(0.1)

