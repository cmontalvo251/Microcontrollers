import time
import board
import digitalio
import os

switch = digitalio.DigitalInOut(board.D7)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True
print('Sleeping for 5 seconds....')
time.sleep(5.0)
if switch.value == False:
    ##Loop through existing files
    FILE_EXISTS = True
    number = 0
    while FILE_EXISTS:
        filename = 'DataFile' + str(number) + '.txt'
        print('Checking for Filename = ',filename)
        try:
            os.stat(filename)
            print('This file exists = ',filename)
            FILE_EXISTS = True
            number+=1
        except:
            FILE_EXISTS = False
            print('Filename Found using =',filename)
    file = open(filename,'w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False
print('Sleeping for another 5 seconds...')q
time.sleep(5.0)
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
