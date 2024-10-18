import time
import board
import analogio
import pwmio
import random
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.5)

#servo stuff
servo = pwmio.PWMOut(board.A3, frequency=50)
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

##SLOTS
o = ["potato","radish"]


###TWO DIFFERENT ANALOG SIGNALS
analog = analogio.AnalogIn(board.A1)
analog2 = analogio.AnalogIn(board.A2)

##STATE MACHINE
STATE = -1
#-1 = print start game
#0 = waiting for a coin
#1 = received a coin, waiting for lever pull
#2 = coin inserted, lever pulled, spin slots
#3 = all slots are the same

###ADVERTISING AND CONNECTED FLAGS
ADVERTISING = False
CONNECTED = False
BLUEPRINT = False
BLUESLEEP = False

while True:
    if ADVERTISING == False and CONNECTED == False:
        print('Not connected')
        print('Look for',ble.name)
        ble.start_advertising(advertisement)
        ADVERTISING = True
    if ble.connected:
        ble.stop_advertising()
        ADVERTISING = False
        CONNECTED = True
    if not ble.connected:
        CONNECTED = False

    val1 = analog.value
    val2 = analog2.value
    #print(time.monotonic(),val2)

    if STATE == -1:
        print('Insert Coin.....')
        STATE = 0
    if STATE == 0:
        pixels.fill((255,255,255))
        val = analog.value
        #print(val)
        if ble.connected:
            if BLUEPRINT == False:
                print('Waiting X seconds for Charlie to open UART on app')
            if BLUESLEEP == False:
                time.sleep(10)
                BLUESLEEP = True
            if BLUEPRINT == False:
                uart_server.write('Insert Coin..')
                print('Sent to App')
                BLUEPRINT = True
        if val > 60000:
            print('Coin has been inserted. Waiting for lever pull...')
            if ble.connected:
                uart_server.write('Coin has been')
                uart_server.write('inserted.')
                uart_server.write('Waiting for')
                uart_server.write('lever pull...')
            else:
                print("You are not connected")
            STATE = 1
    if STATE == 1:
        pixels.fill((255,0,0))
        val = analog2.value
        #print(val)
        if val > 40000:
            STATE = 2
    if STATE == 2:
        pixels.fill((0,255,0))
        a1=random.randrange(0,2)
        a2=random.randrange(0,2)
        a3=random.randrange(0,2)
        print(o[a1],o[a2],o[a3])
        if ble.connected:
            uart_server.write(o[a1]+' '+o[a2]+' '+o[a3])
        else:
            print("You are not connected")
        if a1==a2==a3:
            STATE = 3
        else:
            print('YOU LOOSTTT! SUCKS TO SUCK')
            if ble.connected:
                uart_server.write('YOU LOST')
                uart_server.write('Play a different game')
            else:
                print("You are not connected")
            for x in range(0,4):
                pixels.fill((255,0,0))
                time.sleep(0.2)
                pixels.fill((0,0,0))
                time.sleep(0.2)
            STATE = 4

    if STATE == 3:
        servo.duty_cycle = servo_duty_cycle(1.5)
        print('YOU WINNNNNNNNN QUIT YOUR DAY JOB!!!!')
        if ble.connected:
            uart_server.write('YOU WINN!!!!')
            uart_server.write('Life cannot get any better')
        else:
            print("You are not connected")
        for x in range(0,4):
            pixels.fill((0,0,255))
            time.sleep(0.2)
            pixels.fill((0,0,0))
            time.sleep(0.2)
        STATE = 4

    if STATE == 4:
        print("Move lever back up to play again")
        if ble.connected:
            uart_server.write('Move lever back up to play again. You do want to play right??????????')
        else:
            print('Not connected')
        STATE = 5
        BLUEPRINT = False

    if STATE == 5:
        if analog2.value < 35000:
            STATE = -1
    else:
        servo.duty_cycle = servo_duty_cycle(0.5)
    time.sleep(0.1)
