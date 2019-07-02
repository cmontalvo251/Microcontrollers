# The MIT License (MIT)
#
# Copyright (c) 2019 Brent Rubell for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`adafruit_cursorcontrol`
================================================================================

Mouse cursor for interaction with CircuitPython UI elements.


* Author(s): Brent Rubell

Implementation Notes
--------------------

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""
import displayio

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_Cursor.git"

class Cursor:
    """Mouse cursor interaction for CircuitPython.

    :param ~displayio.Display display: CircuitPython display object.
    :param ~displayio.Group display_group: CircuitPython group object to append the cursor to.
    :param int cursor_speed: Speed of the cursor, in pixels.
    :param int scale: Scale amount for the cursor in both directions.
    :param bool is_hidden: Cursor is hidden on init.

    Example for creating a cursor layer

    .. code-block:: python

        from adafruit_cursorcontrol import Cursor
        # Create the display
        display = board.DISPLAY

        # Create the display context
        splash = displayio.Group(max_size=22)

        # initialize the mouse cursor object
        mouse_cursor = Cursor(display, display_group=splash)
    """
    # pylint: disable=too-many-arguments
    def __init__(self, display=None, display_group=None, is_hidden=False, cursor_speed=5, scale=1):
        self._display = display
        self._scale = scale
        self._speed = cursor_speed
        self._is_hidden = is_hidden
        self._display_grp = display_group
        self._disp_sz = display.height - 1, display.width - 1
        self.generate_cursor()

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        self.deinit()

    def deinit(self):
        """deinitializes the cursor object."""
        self._is_deinited()
        self._scale = None
        self._display_grp.remove(self._cursor_grp)

    def _is_deinited(self):
        """checks cursor deinitialization"""
        if self._scale is None:
            raise ValueError("Cursor object has been deinitialized and can no longer "
                             "be used. Create a new cursor object.")

    @property
    def scale(self):
        """Returns the cursor's scale amount as an integer."""
        return self._scale

    @scale.setter
    def scale(self, scale_value):
        """Scales the cursor by scale_value in both directions.
        :param int scale_value: Amount to scale the cursor by.
        """
        self._is_deinited()
        if scale_value > 0:
            self._scale = scale_value
            self._cursor_grp.scale = scale_value

    @property
    def speed(self):
        """Returns the cursor's speed, in pixels."""
        return self._speed

    @speed.setter
    def speed(self, speed):
        """Sets the speed of the cursor.
        :param int speed: Cursor movement speed, in pixels.
        """
        self._is_deinited()
        if speed > 0:
            self._speed = speed

    @property
    def x(self):
        """Returns the cursor's x-coordinate."""
        return self._cursor_grp.x

    @x.setter
    def x(self, x_val):
        """Sets the x-value of the cursor.
        :param int x_val: cursor x-position, in pixels.
        """
        self._is_deinited()
        if x_val < 0 and not self._is_hidden:
            self._cursor_grp.x = self._cursor_grp.x
        elif x_val > self._disp_sz[1] and not self._is_hidden:
            self._cursor_grp.x = self._cursor_grp.x
        elif not self._is_hidden:
            self._cursor_grp.x = x_val

    @property
    def y(self):
        """Returns the cursor's y-coordinate."""
        return self._cursor_grp.y

    @y.setter
    def y(self, y_val):
        """Sets the y-value of the cursor.
        :param int y_val: cursor y-position, in pixels.
        """
        self._is_deinited()
        if y_val < 0 and not self._is_hidden:
            self._cursor_grp.y = self._cursor_grp.y
        elif y_val > self._disp_sz[0] and not self._is_hidden:
            self._cursor_grp.y = self._cursor_grp.y
        elif not self._is_hidden:
            self._cursor_grp.y = y_val

    @property
    def hide(self):
        """Returns True if the cursor is hidden or visible on the display."""
        return self._is_hidden

    @hide.setter
    def hide(self, is_hidden):
        self._is_deinited()
        if is_hidden:
            self._is_hidden = True
            self._display_grp.remove(self._cursor_grp)
        else:
            self._is_hidden = False
            self._display_grp.append(self._cursor_grp)

    def generate_cursor(self):
        """Generates a cursor icon"""
        self._is_deinited()
        self._cursor_grp = displayio.Group(max_size=1, scale=self._scale)
        self._cur_bmp = displayio.Bitmap(20, 20, 3)
        self._cur_palette = displayio.Palette(3)
        self._cur_palette.make_transparent(0)
        self._cur_palette[1] = 0xFFFFFF
        self._cur_palette[2] = 0x0000
        # left edge, outline
        for i in range(0, self._cur_bmp.height):
            self._cur_bmp[0, i] = 2
        # right diag outline, inside fill
        for j in range(1, 15):
            self._cur_bmp[j, j] = 2
            for i in range(j+1, self._cur_bmp.height - j):
                self._cur_bmp[j, i] = 1
        # bottom diag., outline
        for i in range(1, 5):
            self._cur_bmp[i, self._cur_bmp.height-i] = 2
        # bottom flat line, right side fill
        for i in range(5, 15):
            self._cur_bmp[i, 15] = 2
            self._cur_bmp[i-1, 14] = 1
            self._cur_bmp[i-2, 13] = 1
            self._cur_bmp[i-3, 12] = 1
            self._cur_bmp[i-4, 11] = 1
        self._cur_sprite = displayio.TileGrid(self._cur_bmp,
                                              pixel_shader=self._cur_palette)
        self._cursor_grp.append(self._cur_sprite)
        self._display_grp.append(self._cursor_grp)
