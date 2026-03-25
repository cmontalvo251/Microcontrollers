#This was created for Dawson/Joshua/Eli group in Spring of 2026
import time
import board
import busio
import neopixel
from adafruit_apds9999 import APDS9999

# Setup I2C (this is your SDA/SCL wiring)
i2c = busio.I2C(board.SCL, board.SDA)

#while not i2c.try_lock():
#    pass

#try:
#    while True:
#        print("I2C addresses found:", [hex(device_address) for device_address in i2c.scan()])
#        time.sleep(2)
#except:
#    pass

## Using the code above and I2C device was found at 0x52
#For some reason the code below is defaulting to look for 0x39

# Setup sensor
apds_sensor = APDS9999(i2c)
apds_sensor.light_sensor_enabled = True
apds_sensor.proximity_sensor_enabled = True
apds_sensor.rgb_mode = True

# Setup NeoPixels (10 built-in LEDs)
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10)

# Loop forever
while True:
    proximity = apds_sensor.proximity
    r, g, b, ir = apds_sensor.rgb_ir
    light_level = apds_sensor.calculate_lux(g)
    #print((proximity,r,g,b,ir))
    print((light_level,proximity))

    time.sleep(0.1)
