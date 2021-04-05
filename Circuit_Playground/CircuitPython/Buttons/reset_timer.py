import time
import board
import digitalio

buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

bootuptime = time.monotonic()
while True:
    time_elapsed = time.monotonic() - bootuptime
    if buttonA.value == True:
        bootuptime = time.monotonic()
    #print("Analog Voltage: %f" % getVoltage(analogin))
    print((time_elapsed,))
    time.sleep(0.1)