import time
import board
import adafruit_thermistor

#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
#thermistor = AnalogIn(board.A9) ##If you want analog
thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

while True:
    temp = thermistor.temperature
    #temp = thermistor.value #if you want analog
    print((temp,))
    time.sleep(0.5)