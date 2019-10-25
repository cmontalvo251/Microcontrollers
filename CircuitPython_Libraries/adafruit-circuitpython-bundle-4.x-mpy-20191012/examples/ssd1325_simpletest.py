import time
import board
import busio
import displayio
import adafruit_ssd1325

displayio.release_displays()

# This pinout works on a Metro and may need to be altered for other boards.
spi = busio.SPI(board.SCL, board.SDA)
tft_cs = board.D9
tft_dc = board.D8
tft_reset = board.D7

display_bus = displayio.FourWire(spi, command=tft_dc, chip_select=tft_cs, reset=tft_reset,
                                 baudrate=1000000)
time.sleep(1)
display = adafruit_ssd1325.SSD1325(display_bus, width=128, height=64)
