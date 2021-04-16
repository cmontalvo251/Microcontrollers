"""
This test will initialize the display using displayio and draw a solid green
background, a smaller purple rectangle, and some yellow text.
"""

###Import some stuffs
import displayio
import digitalio
import board
import terminalio
import time
from adafruit_display_text import label
from adafruit_gizmo import tft_gizmo
print('Done with imports')

# Create the TFT Gizmo display
display = tft_gizmo.TFT_Gizmo()
print('Created Display')

# Make the display context
splash = displayio.Group(max_size=3)
display.show(splash)
print('Splash!')

##Make some solid green
#color_bitmap = displayio.Bitmap(240, 240, 1)
#color_palette = displayio.Palette(1)
#color_palette[0] = 0x00FF00 # Bright Green
###Append to the screen like a sprite
#bg_sprite = displayio.TileGrid(color_bitmap,pixel_shader=color_palette,x=0, y=0)
#splash.append(bg_sprite)

# Draw a label
text_group = displayio.Group(max_size=10, scale=2, x=50, y=120)
text = "Hello World!"
text2 = "My name is Bob"
text3 = "and I am a computer"
text4 = "that is going to destroy you"
text5 = "..."
text_array = [text,text2,text3,text4,text5]
text_area = label.Label(terminalio.FONT, text=text, color=0xFFFF00)
text_group.append(text_area) # Subgroup for text scaling
splash.append(text_group)
print('Text')

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN
print('Button')

def scroll():
    for i in range(0,5):
        print(time.monotonic(),text_array[i])
        text_area.text = text_array[i]
        display.refresh()

##Loop forever
print('Go!!!!')
while True:
    if buttonA.value:
        scroll()
    time.sleep(0.1)