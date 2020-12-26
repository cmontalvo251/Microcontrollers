import time
import random
import terminalio
import neopixel
import board
import json
from adafruit_magtag.magtag import MagTag

DEFAULT_SIGN = None  # Set to None to pick, or "Scorpio" (etc) to skip
SHOW_SIGN_IMAGE = True # skip showing the name of the sign/graphic

light_strip = neopixel.NeoPixel(board.D10, 30, auto_write=True)

magtag = MagTag()

# lights up fast
magtag.peripherals.neopixels.brightness = 0.1
magtag.peripherals.neopixel_disable = False # turn on lights

# main text, index 0
magtag.add_text(
    text_font = "Arial-Bold-24.bdf",
    text_position=(
        magtag.graphics.display.width // 2,
        10,
    ),
    text_scale =1,
    line_spacing=1,
    text_anchor_point=(0.5, 0),
)

# button labels, add all 4 in one loop
for x_coord in (10, 75, 150, 220):
    magtag.add_text(
        text_font = "Arial-12.bdf",
        text_position=(x_coord, magtag.graphics.display.height - 10),
        line_spacing=1.0,
        text_anchor_point=(0, 1),
    )

# large horoscope text, index 5
magtag.add_text(
    text_font= "Arial-18.bdf",
    text_position=(10, 10),
    line_spacing=1.0,
    text_scale =1,
    text_wrap=23,
    text_maxlen=60,
    text_anchor_point=(0, 0),
)
# small horoscope text, index 6
magtag.add_text(
    text_font="Arial-12.bdf",
    text_position=(10, 10),
    line_spacing=1.0,
    text_wrap=35,
    text_maxlen=130,
    text_anchor_point=(0, 0),
)

def PLAY_SONG():
    song = ((330, 1), (370, 1), (392, 2), (370, 2), (330, 2), (330, 1),
           (370, 1), (392, 1), (494, 1), (370, 1), (392, 1), (330, 2))
    for notepair in song:
        magtag.peripherals.play_tone(notepair[0], notepair[1] * 0.2)

# now the horoscope!
magtag.set_background(0xFFFFFF) # back to white background

while True:
    magtag.peripherals.neopixels[3] = 0xFF0000
    magtag.peripherals.neopixels[2] = 0xFFFF00
    magtag.peripherals.neopixels[1] = 0x00FF00
    magtag.peripherals.neopixels[0] = 0x0000FF
    magtag.set_text("Merry Christmas!!", 5)
    light_strip.fill(0xFF0000)
    # refresh horoscope
    time.sleep(0.1)