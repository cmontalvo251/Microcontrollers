#IMPORT MODULES
#For Setup
import time
import busio
import board
import digitalio
import pwmio

##SOUND BEEPING
# Define a list of tones/music notes to play.
TONE_FREQ = [ 262,  # C4
              294,  # D4
              330,  # E4
              349,  # F4
              392,  # G4
              440,  # A4
              494 ] # B4

# Create piezo buzzer PWM output.
buzzer = pwmio.PWMOut(board.SPEAKER, variable_frequency=True)

# Start at the first note and start making sound.
buzzer.frequency = TONE_FREQ[0]
buzzer.duty_cycle = 2**15  # 32768 value is 50% duty cycle, a square wave.

# Enable the speaker
speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
speaker_enable.direction = digitalio.Direction.OUTPUT
speaker_enable.value = True

def PLAY_SOUND(i):
    # Play tones going from start to end of list.
    buzzer.frequency = TONE_FREQ[i]

#For Sensors Used
import adafruit_lis3dh #onboard accelerometer
from adafruit_bme280 import basic as adafruit_bme280  #external pressure sensor
print('Wiring for Pressure sensor')
print('BME -> CPB')
print('VIN -> 3.3V')
print('GND -> GND')
print('SCK -> SCL')
print('SDI -> SDA')
import adafruit_thermistor #on board temperature sensor

#For an Indicator
import neopixel

#For Bluetooth
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

###SETUP LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##SETUP SWITCH
switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

#SETUP PIXELS
pixel_brightness = 0.25
pixels = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=pixel_brightness)

#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
#thermistor = AnalogIn(board.A9) ##If you want analog
thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

##SETUP THE ACCELEROMETER
##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_16_G

##And the external presure sensor
i2c = busio.I2C(board.SCL, board.SDA)
print(dir(adafruit_bme280))
pressure_sensor = adafruit_bme280.Adafruit_BME280_I2C(i2c)

####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

##MAKE A COLOR WHEEL
colors = [(255,0,0),(0,255,0),(0,0,255)]
c = 0

###CHECK FOR OPENING FILE
if switch.value == False:
    file = open('Launch_Data_File.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

###BOOLEANS
ADVERTISING = False

sound_ctr = 0

BEEP = False

#INFINITE WHILE LOOP
while True:
    ##RESET PIXELS
    pixels[0] = (0,0,0)
    led.value = not led.value

    ##PLAY a sound
    if BEEP:
        buzzer.duty_cycle = 2**15
        PLAY_SOUND(sound_ctr)
        sound_ctr+=1
        if sound_ctr >= len(TONE_FREQ):
            sound_ctr = 0
    else:
        buzzer.duty_cycle = 0

    ##GET CURRENT TIME
    t = time.monotonic()

    ##GET THE CURRENT ACCEL
    x,y,z = lis3dh.acceleration

    #and PTH sensor
    p = pressure_sensor.pressure
    Te = pressure_sensor.temperature
    rH = pressure_sensor.humidity

    #GET THE TEMPERATURE from the onboard sensor
    T = thermistor.temperature

    ##PRINT TO STDOUT
    print((t,x,y,z,p,rH,Te,T))

    # Advertise when not connected.
    if not ble.connected:
        if ADVERTISING == False:
            ble.start_advertising(advertisement)
            ADVERTISING = True
        else:
            print('Advertising, Look for ',ble.name)
    else:
        #Stop advertising once connected
        ble.stop_advertising()
        ADVERTISING = False
        uart_server.write('{},{},{},{},{},{},{},{}\n'.format(t,x,y,z,p,rH,Te,T))

    ##CHECK AND SEE IF SWITCH IS THROWN
    if switch.value == False:
        BEEP = True
        print('Logging Data')
        ##CHANGE COLOR EVERY TIME YOU TAKE DATA
        pixels[0] = colors[c]
        c+=1
        if c >= len(colors):
            c = 0
        #PRINT TO A FILE
        #print((t,x,y,z,p,rH,Te,T))
        output = str(t) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(p) + " " + str(rH) + " " + str(Te) + " " + str(T) + str('\n')
        file.write(output)
        file.flush()
    else:
        BEEP = False
        print('File is not open. Flip the switch. Then hit reset to start logging')
        if FILEOPEN == True:
            print('FILE CLOSED!!! Hit reset then copy data')
            file.close()
            FILEOPEN = False
        ##Wait 0.1 seconds for 10 hz data rate
        time.sleep(0.1)