import board
import displayio
from adafruit_display_shapes.rect import Rect
from adafruit_display_shapes.circle import Circle
from adafruit_display_shapes.roundrect import RoundRect
from adafruit_display_shapes.triangle import Triangle

# Make the display context
splash = displayio.Group(max_size=10)
board.DISPLAY.show(splash)

# Make a background color fill
color_bitmap = displayio.Bitmap(320, 240, 1)
color_palette = displayio.Palette(1)
color_palette[0] = 0xFFFFFF
bg_sprite = displayio.TileGrid(color_bitmap, x=0, y=0,
                               pixel_shader=color_palette)
splash.append(bg_sprite)
##########################################################################

triangle = Triangle(170, 50, 120, 140, 210, 160, fill=0x00FF00, outline=0xFF00FF)
splash.append(triangle)

rect = Rect(80, 20, 41, 41, fill=0x0)
splash.append(rect)

circle = Circle(100, 100, 20, fill=0x00FF00, outline=0xFF00FF)
splash.append(circle)

rect2 = Rect(50, 100, 61, 81, outline=0x0, stroke=3)
splash.append(rect2)

roundrect = RoundRect(10, 10, 61, 81, 10, fill=0x0, outline=0xFF00FF, stroke=6)
splash.append(roundrect)


while True:
    pass
