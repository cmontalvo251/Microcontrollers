# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

import time
import board
import adafruit_hts221

i2c = board.I2C()
hts = adafruit_hts221.HTS221(i2c)

while True:
    print("Relative Humidity: %.2f %% rH" % hts.relative_humidity)
    print("Temperature: %.2f C" % hts.temperature)
    print("")
    time.sleep(1)
