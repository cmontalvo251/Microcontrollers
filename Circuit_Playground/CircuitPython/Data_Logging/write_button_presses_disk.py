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
else:
    print('Not opening file for writing')

while True:
    led.value = True
    print(time.monotonic(),int(buttonA.value))
    if switch.value == False:
        print('Writing Data to Disk')
        output = str(time.monotonic()) + " " + str(int(buttonA.value)) + str('\n')
        file.write(output)
        file.flush()
        led.value = False
    else:
        print('Not logging data. Flip the switch and then hit reset')
    time.sleep(1) #sleep for so many seconds between measurements