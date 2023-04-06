import time
import board
import busio
import digitalio
import adafruit_lis3dh
import adafruit_lis3mdl
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
from adafruit_lsm6ds.lsm6ds33 import LSM6DS33
from adafruit_lsm6ds import GyroRange
import neopixel


####Setup blue tooth
ble = BLERadio()
uart_server = UARTService()
advertisement = ProvideServicesAdvertisement(uart_server)
print('Bluetooth Setup')

i2c = busio.I2C(board.SCL, board.SDA)
accelgyro = LSM6DS33(i2c,address=0x6a)
accelgyro.gyro_range = GyroRange.RANGE_2000_DPS

#Set up pixels
pixel_brightness = 0.25
pixels = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=pixel_brightness)

# switch = digitalio.DigitalInOut(board.D5)  # For Feather M0/M4 Express
##Button Presses
print(dir(board))
switch = digitalio.DigitalInOut(board.D7)  # For Circuit Playground Express
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

##Accelerometer is hooked up to SDA/SCL which is I2C
#i2cCPB = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
#_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
#lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2cCPB, address=0x19, int1=_int1)
#lis3dh.range = adafruit_lis3dh.RANGE_8_G

if switch.value == True:
    print('Not taking data')
    pixels.fill((255,0,0))
    DATA = False
else:
    print('Taking data....')
    pixels.fill((0,255,0))
    DATA = True
    file = open('Disc_Data.txt','w')
#while not i2c.try_lock():
#    pass

#try:
#    while True:
#        print("I2C addresses found:", [hex(device_address) for device_address in i2c.scan()])
#        time.sleep(2)

#finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
#    i2c.unlock()
## 0x1e and 0x6a on my broken one
## 0x1c and 0x6a on a working one - 6a is the accelerometer/gyro and the 1e/1c is the magneometer
prevtime = time.monotonic()
val = 255
ADVERTISING = False
while True:
    t = time.monotonic()
    gx,gy,gz = accelgyro.gyro
    ax,ay,az = accelgyro.acceleration
    #print(t,ax,ay,az,gx,gy,gz)
    print((t,t-prevtime,ax,ay,az,gx,gy,gz,switch.value))
    val -= 10
    if val < 0:
        val = 255
    if DATA:
        print('Taking data')
        pixels.fill((0,val,0))
        output = str(t) + " " + str(ax) + " " + str(ay) + " " + str(az) + " " + str(gx) + " " + str(gy) + " " + str(gz) + str('\n')
        file.write(output)
        file.flush()
        if switch.value == True:
            DATA = 0
            file.close()
    else:
        print('Not taking data. Flip the switch and hit reset...')
        pixels.fill((val,0,0))
        if switch.value == False:
            print('Intentionally writing bad code to throw an error....')
            file.write('')
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
        uart_server.write('{}\n'.format(gz))
    #print((gx,gy,gz))
    prevtime = t
    time.sleep(0.02)