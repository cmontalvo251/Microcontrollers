import time
import board
import digitalio

switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

if switch.value == False:
    file = open('Method3.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False

while True:
    led.value = not led.value
    print(time.monotonic(),int(buttonA.value))
    if switch.value == False:
        print('Writing Data to Disk')
        output = str(time.monotonic()) + " " + str(int(buttonA.value)) + str('\n')
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
