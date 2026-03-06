###CODE TESTED ON A CPX AND CPB - DOES NOT WORK DO NOT KNOW WHAT TO DO PLEASE HELP

import time
import board
import busio
import storage
import digitalio
import adafruit_sdcard

# -----------------------------
# SD CARD SETUP
# -----------------------------
##board.SCK - A3  (CLK)
##board.MOSI - A1 (DO)
##board.MISO - A2 (DI)
##VCC - VOUT
##GND - GND
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)

# Chip Select MUST be DigitalInOut
##CS - A7
print(dir(board))
cs = digitalio.DigitalInOut(board.TX)
cs.direction = digitalio.Direction.OUTPUT

# Initialize SD card
sd = adafruit_sdcard.SDCard(spi, cs)
vfs = storage.VfsFat(sd)
storage.mount(vfs, "/sd")

print("SD card mounted!")

# Create a new file
filename = "/sd/accel_log.csv"
logfile = open(filename, "w")

# Write CSV header
logfile.write("time,ax,ay,az\n")
logfile.flush()

print("Logging to", filename)

# -----------------------------
# MAIN LOOP
# -----------------------------
x = 0
while True:
    x += 1
    t = time.monotonic()

    line = "{:.3f},{:.3f}\n".format(t,x)
    logfile.write(line)
    logfile.flush()

    print(line, end="")
    time.sleep(0.05)
