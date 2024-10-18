
import time
import board
import digitalio
import busio
import math
import neopixel
import adafruit_lis3dh
from adafruit_crickit import crickit


##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G


try:
    from audiocore import WaveFile
except ImportError:
    from audioio import WaveFile

try:
    from audioio import AudioOut
except ImportError:
    try:
        from audiopwmio import PWMAudioOut as AudioOut
    except ImportError:
        pass  # not always supported by every board!

# enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

from adafruit_bluefruit_connect.packet import Packet
# Only the packet classes that are imported will be known to Packet.
from adafruit_bluefruit_connect.button_packet import ButtonPacket
from adafruit_bluefruit_connect.color_packet import ColorPacket

# Prep the status LED on the CPB
red_led = digitalio.DigitalInOut(board.D13)
red_led.direction = digitalio.Direction.OUTPUT

def take_data():
    print('Data collection')
    time.sleep(10)
    timestart = time.monotonic()
    for i in range(0,100):
        t = time.monotonic()
        x,y,z = lis3dh.acceleration
        print((t,x,y,z,i))
        uart_service.write('{},{},{}\n'.format(x,y,z))
        if i > 10:
            motor_1.throttle = FWD
            motor_2.throttle = FWD
        if i > 40:
            motor_1.throttle = REV
            motor_2.throttle = REV
        if i > 70:
            motor_1.throttle = FWD
            motor_2.throttle = 0
        if i > 85:
            motor_1.throttle = 0
            motor_2.throttle = FWD
        time.sleep(0.1)
    motor_1.throttle = 0
    motor_2.throttle = 0

def PLAY_SOUND(filename):
    print("Playing file: " + filename)
    wave_file = open(filename, "rb")
    with WaveFile(wave_file) as wave:
        with AudioOut(board.SPEAKER) as audio:
            audio.play(wave)
            while audio.playing:
                pass
    return

ble = BLERadio()
uart_service = UARTService()
advertisement = ProvideServicesAdvertisement(uart_service)

# motor setup
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2

FWD = 0.5
REV = -0.5

neopixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.1)
RED = (200, 0, 0)
GREEN = (0, 200, 0)
BLUE = (0, 0, 200)
PURPLE = (120, 0, 160)
YELLOW = (100, 100, 0)
AQUA = (0, 100, 100)
BLACK = (0, 0, 0)
color = PURPLE  # current NeoPixel color
neopixels.fill(color)

print("Who's Ya Caddy")
print("Use Adafruit Bluefruit app to connect")
while True:
    neopixels[0] = BLACK
    neopixels.show()
    ble.start_advertising(advertisement)
    while not ble.connected:
        # Wait for a connection.
        pass
    # set a pixel blue when connected
    neopixels[0] = BLUE
    neopixels.show()
    while ble.connected:
        if uart_service.in_waiting:
            # Packet is arriving.
            red_led.value = False  # turn off red LED
            packet = Packet.from_stream(uart_service)
            if isinstance(packet, ColorPacket):
                # Change the color.
                color = packet.color
                neopixels.fill(color)

            # do this when buttons are pressed
            if isinstance(packet, ButtonPacket) and packet.pressed:
                red_led.value = True  # blink to show packet has been received
                if packet.button == ButtonPacket.UP:
                    neopixels.fill(color)
                    PLAY_SOUND('fwd.wav')
                    motor_1.throttle = FWD
                    motor_2.throttle = FWD
                elif packet.button == ButtonPacket.DOWN:
                    neopixels.fill(color)
                    PLAY_SOUND('back.wav')
                    motor_1.throttle = REV
                    motor_2.throttle = REV
                elif packet.button == ButtonPacket.RIGHT:
                    color = YELLOW
                    neopixels.fill(color)
                    motor_2.throttle = 0
                    motor_1.throttle = FWD
                elif packet.button == ButtonPacket.LEFT:
                    color = YELLOW
                    neopixels.fill(color)
                    motor_2.throttle = FWD
                    motor_1.throttle = 0
                elif packet.button == ButtonPacket.BUTTON_1:
                    neopixels.fill(RED)
                    motor_1.throttle = 0.0
                    motor_2.throttle = 0.0
                    time.sleep(0.5)
                    neopixels.fill(color)
                elif packet.button == ButtonPacket.BUTTON_2:
                    color = GREEN
                    neopixels.fill(color)
                elif packet.button == ButtonPacket.BUTTON_3:
                    color = BLUE
                    neopixels.fill(color)
                elif packet.button == ButtonPacket.BUTTON_4:
                    take_data()
            # do this when some buttons are released
            elif isinstance(packet, ButtonPacket) and not packet.pressed:
                if packet.button == ButtonPacket.UP:
                    neopixels.fill(RED)
                    motor_1.throttle = 0
                    motor_2.throttle = 0
                if packet.button == ButtonPacket.DOWN:
                    neopixels.fill(RED)
                    motor_1.throttle = 0
                    motor_2.throttle = 0
                if packet.button == ButtonPacket.RIGHT:
                    neopixels.fill(RED)
                    motor_1.throttle = 0
                    motor_2.throttle = 0
                if packet.button == ButtonPacket.LEFT:
                    neopixels.fill(RED)
                    motor_1.throttle = 0
                    motor_2.throttle = 0
