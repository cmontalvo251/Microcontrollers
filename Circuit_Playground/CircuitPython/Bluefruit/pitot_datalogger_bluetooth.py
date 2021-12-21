#IMPORT MODULES
import time
import busio
import board
import digitalio
import adafruit_lis3dh
import adafruit_thermistor
import analogio
import math
import neopixel
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

#Set up pixels
pixel_brightness = 0.25
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=pixel_brightness)

##SETUP Button A
button_a = digitalio.DigitalInOut(board.D4)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.DOWN

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
Vcal = 0
Ucal = 0
U0 = 0
ctr = 0
N = 10
ctrU = N
Uf = 0
f = 0.9
ADVERTISING = False

#INFINITE WHILE LOOP
while True:
    #Get Voltage
    V = getVoltage(pin)

    ##Compute Windspeed
    U = windspeed(V,V0)
    ##Output
    Uout = U-U0

    #Filter Windspeed
    Uf = (1-f)*Uout + f*Uf

    #Calibration
    if ctr < N:
        pixels.fill((255,0,0))
        Vcal += V
        ctr+=1
        ctrU = -1
    else:
        V0 = Vcal/N
    if ctrU < N:
        if ctrU > -1:
            pixels.fill((0,255,0))
            Ucal += U
        ctrU+=1
    else:
        U0 = Ucal/N
    if ctrU == N and ctr == 10:
        #Turn on all pixels off
        pixels.fill((0,0,0))

    ##Check for Button press to calibrate
    if button_a.value==True:
        ctr = 0
        Vcal = 0

    # Advertise when not connected.
    if not ble.connected:
        print('Not connected')
        if ADVERTISING == False:
            ble.start_advertising(advertisement)
            ADVERTISING = True
        else:
            print('Advertising')
    else:
        #Stop advertising once connected
        print('Connected')
        ble.stop_advertising()
        ADVERTISING = False
        uart_server.write('{},{}\n'.format(Uout,Uf))

    ##PRINT TO STDOUT
    #print(V,V0,V-V0,Vcal,ctr,ctrU,U,U0,U-U0,Ucal,Uf)
    print((Uout,Uf))

    ##Wait 0.2 seconds for 5 hz data rate
    time.sleep(0.2)# Write your code here :-)