#IMPORT MODULES
import time
import busio
import board
import digitalio
import analogio
import math
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

#Set up pixels
pixel_brightness = 1.0
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

##SETUP Button A
button_a = digitalio.DigitalInOut(board.D4)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.DOWN

##SETUP Button B
button_b = digitalio.DigitalInOut(board.D5)
button_b.direction = digitalio.Direction.INPUT
button_b.pull = digitalio.Pull.DOWN

switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP
if switch.value == False:
    file = open('CPB_Datalog.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

##SETUP ANALOGIO
pin = analogio.AnalogIn(board.A4)
#Function to convert to decimal
def bin2dec(value):
    return value*5.0/(2**16)
#Function to calibrate
def getVoltage(pin):
    return bin2dec(pin.value)

##Function to convert to windspeed
def windspeed(V,V0):
    ##Differential voltage
    dV = V-V0
    #Pressure in Pa
    dP = 1000*dV
    ##Density
    rho = 1.225
    ##Check for negative
    if dP < 0:
        vel = -math.sqrt(-2*dP/rho)
    else:
        vel = math.sqrt(2*dP/rho)
    return vel

#Globals
V0 = 0
U0 = 0
Uf = 0
f = 0.9
ADVERTISING = False

#INFINITE WHILE LOOP
startTime = time.monotonic()
while True:
    #Get time
    t = time.monotonic()-startTime

    #Get Voltage
    V = getVoltage(pin)

    ##Compute Raw Windspeed
    U = windspeed(V,V0)
    ##Output
    Uout = U-U0

    #Filter Windspeed
    Uf = (1-f)*Uout + f*Uf

    ##Check for Button press to calibrate
    if button_a.value==True:
        V0 = V
        pixels.fill((255,0,0))
        time.sleep(2)
    if button_b.value == True:
        U0 = Uf
        pixels.fill((0,255,0))
        time.sleep(2)
    pixels.fill((0,0,0))

    # Advertise when not connected.
    if not ble.connected:
        if ADVERTISING == False:
            ble.start_advertising(advertisement)
            ADVERTISING = True
            timeStartA = time.monotonic()
            val = True
        else:
            print('Not connected: Advertising',ble.name,time.monotonic()-timeStartA)
            val = not val
            pixels.fill((int(val),int(val),int(val)))
    else:
        #Stop advertising once connected
        ble.stop_advertising()
        ADVERTISING = False
        uart_server.write('{},{}\n'.format(Uout,Uf))
    #PRINT TO A FILE
    if FILEOPEN:
        output = str(t) + " " + str(Uout) + " " + str(Uf) + str('\n')
        file.write(output)
        file.flush()
        led.value = not led.value
    else:
        led.value = True
    if switch.value == True and FILEOPEN:
        print('File closed')
        file.close()
        FILEOPEN = False

    ##PRINT TO STDOUT
    #print(V,V0,V-V0,Vcal,ctr,ctrU,U,U0,U-U0,Ucal,Uf)
    print((Uout,Uf))

    ##Wait 0.5 seconds for 2 hz data rate
    time.sleep(0.5)# Write your code here :-)