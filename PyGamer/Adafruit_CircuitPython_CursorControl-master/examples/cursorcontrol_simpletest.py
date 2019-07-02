import time
import board
import displayio
from adafruit_cursorcontrol.cursorcontrol import Cursor
from adafruit_cursorcontrol.cursorcontrol_cursormanager import CursorManager

# Create the display
display = board.DISPLAY

# Create the display context
splash = displayio.Group(max_size=5)

# initialize the mouse cursor object
mouse_cursor = Cursor(display, display_group=splash)

# initialize the cursormanager
cursor = CursorManager(mouse_cursor)

# show displayio group
display.show(splash)

while True:
    cursor.update()
    if cursor.is_clicked:
        if mouse_cursor.hide:
            mouse_cursor.hide = False
        else:
            mouse_cursor.hide = True
    time.sleep(0.01)
