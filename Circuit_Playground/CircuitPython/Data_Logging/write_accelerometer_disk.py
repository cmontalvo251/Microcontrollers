import time
import board
import digitalio
import busio
import adafruit_lis3dh

switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

if switch.value == False:
    file = open('Method3.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

while True:
    led.value = not led.value
    x,y,z = lis3dh.acceleration
    print(time.monotonic(),x,y,z)
    if switch.value == False:
        print('Writing Data to Disk')
        output = str(time.monotonic()) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str('\n')
        file.write(output)
        file.flush()
        led.value = False
    else:
        print('Not logging data. Flip the switch and then hit reset')
        if FILEOPEN == True:
            #This means the file used to be open and we just closed it so we wil properly close the file
            print('FILE CLOSED!!!')
            file.close()
            FILEOPEN = False
    time.sleep(0.2) #sleep for so many seconds between measurements