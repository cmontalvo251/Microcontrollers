import board
import digitalio
import neopixel
import adafruit_matrixkeypad

class _NeoPixelArray:
    def __init__(self, pin, *, width, height, rotation=0):
        self._width = width
        self._height = height
        self._neopixel = neopixel.NeoPixel(pin, width * height, auto_write=False)
        if rotation % 90 != 0:
            raise ValueError("Only 90 degree rotations supported")
        self._rotation = rotation % 360

    def __setitem__(self, index, value):
        if not isinstance(index, tuple) or len(index) != 2:
            raise IndexError("Index must be tuple")

        if self._rotation == 0 or self._rotation == 180:
            offset = self._width * index[1] + index[0]
            if self._rotation == 180:
                offset = self._width * self._height - offset - 1
        elif self._rotation == 270:
            offset = self._width * index[0] + (self._width - index[1] - 1)
        elif self._rotation == 90:
            offset = self._width * (self._height - index[0] - 1) + index[1]

        self._neopixel[offset] = value
        self._neopixel.show()

class TrellisM4Express:
    def __init__(self, rotation=0):
        self._rotation = rotation
        self.pixels = _NeoPixelArray(board.NEOPIXEL, width=8, height=4, rotation=rotation)

        print(dir(board))
        cols = []
        for x in range(8):
            d = digitalio.DigitalInOut(getattr(board, "COL{}".format(x)))
            cols.append(d)

        rows = []
        for y in range(4):
            d = digitalio.DigitalInOut(getattr(board, "ROW{}".format(y)))
            rows.append(d)

        key_names = []
        for y in range(8):
            row = []
            for x in range(4):
                if rotation == 0:
                    coord = (x,y)
                elif rotation == 180:
                    coord = (3 - x, 7 - y)
                elif rotation == 90:
                    coord = (3 - x, y)
                elif rotation == 270:
                    coord = (x, 7 - y)
                row.append(coord)
            key_names.append(row)

        self._matrix = adafruit_matrixkeypad.Matrix_Keypad(cols, rows, key_names)

    @property
    def pressed_keys(self):
        return self._matrix.pressed_keys
