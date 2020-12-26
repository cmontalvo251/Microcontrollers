# SPDX-FileCopyrightText: 2020 Adafruit Industries
#
# SPDX-License-Identifier: Unlicense
import time
import random
import terminalio
import json
from adafruit_magtag.magtag import MagTag

SHOW_INTRO = True   # Whether to show the digikey logo + intro text
PLAY_SONG = True    # shhhh!
DEFAULT_SIGN = None  # Set to None to pick, or "Scorpio" (etc) to skip
SHOW_SIGN_IMAGE = True # skip showing the name of the sign/graphic
PLAINFONT = False     # Use built in font if True
# if we have wifi, we'll get a horoscope online!
DATA_SOURCE = "https://aztro.sameerkumar.website/?day=today&sign="

magtag = MagTag()

# lights up fast
magtag.peripherals.neopixels.brightness = 0.1
magtag.peripherals.neopixel_disable = False # turn on lights
if SHOW_INTRO:
    magtag.set_background("digikey.bmp")
    magtag.peripherals.neopixels.fill(0x800000) # red!
    magtag.refresh()
    print("Thank you Digi-Key!")

signs = (("Aquarius", (0, 0, 255)), # blue
         ("Pisces", (144, 240, 144)), # light green
         ("Aries", (255, 0, 0)), # red
         ("Taurus", (0, 255, 0)), # green
         ("Gemini", (128, 128, 0)), # yellow
         ("Cancer", (128, 128, 128)), # white
         ("Leo", (255, 215, 0)), # gold
         ("Virgo", (82, 111, 53)), # dark green
         ("Libra", (255,192,203)), # pink
         ("Scorpio", 0), # black!
         ("Sagittarius", (128,0,128)), # purple
         ("Capricorn", (165,42,42)) # brown
)
horoscopes = json.loads(open("horoscopes.json").read())
#print(horoscopes)

# main text, index 0
magtag.add_text(
    text_font = terminalio.FONT if PLAINFONT else "Arial-Bold-24.bdf",
    text_position=(
        magtag.graphics.display.width // 2,
        10,
    ),
    text_scale = 3 if PLAINFONT else 1,
    line_spacing=1,
    text_anchor_point=(0.5, 0),
)

# button labels, add all 4 in one loop
for x_coord in (10, 75, 150, 220):
    magtag.add_text(
        text_font = terminalio.FONT if PLAINFONT else "Arial-12.bdf",
        text_position=(x_coord, magtag.graphics.display.height - 10),
        line_spacing=1.0,
        text_anchor_point=(0, 1),
    )

# large horoscope text, index 5
magtag.add_text(
    text_font= terminalio.FONT if PLAINFONT else "Arial-18.bdf",
    text_position=(10, 10),
    line_spacing=1.0,
    text_scale = 2 if PLAINFONT else 1,
    text_wrap=23,
    text_maxlen=60,
    text_anchor_point=(0, 0),
)
# small horoscope text, index 6
magtag.add_text(
    text_font= terminalio.FONT if PLAINFONT else "Arial-12.bdf",
    text_position=(10, 10),
    line_spacing=1.0,
    text_wrap=35,
    text_maxlen=130,
    text_anchor_point=(0, 0),
)

if SHOW_INTRO:
    magtag.set_background(0xFFFFFF)    # set to white background
    magtag.set_text("Welcome to\nADABOX 017") # let me introduce myself
    if PLAY_SONG:
        song = ((330, 1), (370, 1), (392, 2), (370, 2), (330, 2), (330, 1),
                (370, 1), (392, 1), (494, 1), (370, 1), (392, 1), (330, 2))
        for notepair in song:
            magtag.peripherals.play_tone(notepair[0], notepair[1] * 0.2)

try:
    magtag.network.connect()
except (ConnectionError) as e:
    print(e)
    print("Continuing without WiFi")

# initial instructions
selection = None

# group the signs by threes for selection
signgroups = [[], [], [], []]
for i, s in enumerate(signs):
    name = s[0]
    # if we already have a default, lets just use that!
    if DEFAULT_SIGN == name:
        selection = i
    if len(name) > 7:
        name = name[0:6]+"."
    signgroups[i // 3].append(name)

# wait till they press a button
magtag.set_background(0xFFFFFF)
while not isinstance(selection, int):
    magtag.peripherals.neopixels.fill(0x0F0F0F)
    if selection is None:  # no group selected
        magtag.set_text("Choose your sign", 0, False)
        magtag.set_text("\n".join(signgroups[0]), 1, False)
        magtag.set_text("\n".join(signgroups[1]), 2, False)
        magtag.set_text("\n".join(signgroups[2]), 3, False)
        magtag.set_text("\n".join(signgroups[3]), 4)
    else:  # group selected, display each option!
        magtag.set_text(selection[0], 1, False)
        magtag.set_text(selection[1], 2, False)
        magtag.set_text(selection[2], 3, False)
        magtag.set_text("Back", 4)

    magtag.peripherals.neopixels.fill(0x0)
    while True:
        for i, b in enumerate(magtag.peripherals.buttons):
            if not b.value:
                magtag.peripherals.neopixels.fill(0x0F0F0F)
                print("Button %c pressed" % chr((ord("A") + i)))
                if selection is None:
                    selection=signgroups[i]
                elif i == 3: # 'back'
                    selection = None
                else:
                    group = signgroups.index(selection) # find the group
                    selection = group * 3 + i
                break
        else:
            continue
        break

# clear selections
for i in range(1,5):
    magtag.set_text("", i, False)

# convert the number selected back to the full sign description
selection = signs[selection]
print(selection)

if SHOW_SIGN_IMAGE:
    # set the big text
    magtag.set_text("\n"+selection[0])
    # set the neopixel colors for that sign
    magtag.peripherals.neopixels.fill(selection[1])
    # next, display the lovely graphic
    filename = "images/adafruit_"+selection[0].lower()+".bmp"
    magtag.set_background(filename) # show a nice image
magtag.set_text("") # erase intructional text

def display_fortune(sign, text, color):
    magtag.peripherals.neopixels[3] = 0xFF0000
    magtag.peripherals.neopixels[2] = 0xFFFF00
    magtag.peripherals.neopixels[1] = 0x00FF00
    magtag.peripherals.neopixels[0] = 0x0000FF
    try:
        url = DATA_SOURCE + sign
        print(url)
        response = magtag.network.requests.post(url)
        horo = response.json()
        print(horo)
        magtag.set_text("Press any button to request another", 1, False)
        magtag.set_text("", 5, False)
        magtag.set_text(horo["description"], 6)
    except:
        magtag.set_text("Press any button for a new fortune", 1, False)
        magtag.set_text("", 6, False)
        magtag.set_text(text, 5)
    magtag.peripherals.neopixels.fill(0x0) # no lights!

# now the horoscope!
magtag.set_background(0xFFFFFF) # back to white background
display_fortune(selection[0], random.choice(horoscopes)["text"], selection[1])

while True:
    # refresh horoscope
    for i, b in enumerate(magtag.peripherals.buttons):
        if not b.value:
            display_fortune(selection[0], random.choice(horoscopes)["text"], selection[1])
            break
    time.sleep(0.1)