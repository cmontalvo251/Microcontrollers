"""Simple test script for 2.7" 264x176 Tri-Color display shield

Supported products:
  * Adafruit 2.7" Tri-Color ePaper Display Shield
    * https://www.adafruit.com/product/4229
  """

import time
import board
import displayio
import adafruit_il91874

displayio.release_displays()

spi = board.SPI()
epd_cs = board.D10
epd_dc = board.D9

display_bus = displayio.FourWire(spi, command=epd_dc, chip_select=epd_cs, baudrate=1000000)
time.sleep(1)

display = adafruit_il91874.IL91874(display_bus, width=264, height=176, highlight_color=0xff0000,
                                   rotation=90)

g = displayio.Group()

f = open("/display-ruler.bmp", "rb")

pic = displayio.OnDiskBitmap(f)
t = displayio.TileGrid(pic, pixel_shader=displayio.ColorConverter())
g.append(t)

display.show(g)

display.refresh()

print("refreshed")

time.sleep(120)
