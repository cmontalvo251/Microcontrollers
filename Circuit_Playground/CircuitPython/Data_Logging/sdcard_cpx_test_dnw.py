import time
import board
import busio
import storage
import digitalio
import adafruit_sdcard

##This current code has been tested on a CircuitPlayground Bluefruit but should work no problema
#on a circuit playground express.

#This requires the use of a micro sd card breakout board from adafruit PID:254 - https://www.adafruit.com/product/254
#and a SD card formatted to FAT and contains an sd/ folder.

# -----------------------------
# SD CARD SETUP
# -----------------------------
##board.SCK = A1 -> CLK
##board.MOSI = A3 -> DI
##board.MISO = A2 -> DO
##board.AUDIO        -> CS
##VCC - VOUT
##GND - GND
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
cs = digitalio.DigitalInOut(board.AUDIO)
cs.direction = digitalio.Direction.OUTPUT
sd = adafruit_sdcard.SDCard(spi, cs)
vfs = storage.VfsFat(sd)
storage.mount(vfs, "/sd")
print("SD card mounted!")

#BUTTON & LED SETUP
buttonA = digitalio.DigitalInOut(board.BUTTON_A)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

#FILE SETUP
filename = "/sd/accel_log.csv"
logfile = open(filename, "a")
FILEOPEN = True
print("Press Button A to record.")

# -----------------------------
# MAIN LOOP
# -----------------------------
counter = 0
RECORD = False
DATARECORDED = False
while True:
    if buttonA.value == True:
        RECORD = not RECORD
        time.sleep(0.5)
    if RECORD:
        led.value = True

        t = time.monotonic()
        counter += 1

        line = "{:.3f},{}\n".format(t, counter)
        logfile.write(line)
        logfile.flush()

        print("Recording:", line.strip())
        
        DATARECORDED = True
    else:
        led.value = False
        if DATARECORDED == True and FILEOPEN == True:
            logfile.close()
            FILEOPEN = False
            print('File closed')
        if FILEOPEN:
            print('Not recording. Press A to record')
        else:
            print('Press reset to log again')
    time.sleep(0.1)
