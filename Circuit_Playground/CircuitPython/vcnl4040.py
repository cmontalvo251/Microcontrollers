import time
import board
import adafruit_vcnl4040
from analogio import AnalogOut

i2c = board.I2C()
sensor = adafruit_vcnl4040.VCNL4040(i2c)
pin = AnalogOut(board.SPEAKER)

while True:
    print("Proximity:", sensor.proximity,sensor.light,sensor.white)

    #print("Light:", sensor.light)
    #print("White:", sensor.white)
    time.sleep(0.3)
    if sensor.proximity > 5:
        print('Run Motor')
        pin.value = 65535
    else:
        pin.value = 0