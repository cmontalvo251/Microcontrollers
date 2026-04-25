import time
import board
from analogio import AnalogIn
import adafruit_thermistor
import busio
import digitalio
import adafruit_lis3dh

##Analog Read on Photocell - Thankfully this is a simple linear conversion to Lux
photocell = AnalogIn(board.A8)

#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
#thermistor = AnalogIn(board.TEMPERATURE) ##If you want analog
thermistor = adafruit_thermistor.Thermistor(board.TEMPERATURE, 10000, 10000, 25, 3950)

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

while True:
    #print("Analog Voltage: %f" % getVoltage(analogin))
    light = photocell.value*330/(2**16)
    temp = thermistor.temperature
    #temp = thermistor.value #if you want analog
    #Accelerometer
    x,y,z = lis3dh.acceleration
    print("Time (sec) = ",time.monotonic(),"Light (Lux) =",light,"Temp(C) =",temp,"Accel (m/s^2) = ",x,y,z)
    time.sleep(0.5)