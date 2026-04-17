import time
import board
#import adafruit_thermistor
import analogio
import math
#Temperature Sensor is also analog but there is a better way to do it since voltage to temperature
#Is nonlinear and depends on series resistors and b_coefficient (some heat transfer values)
pin = analogio.AnalogIn(board.A9) ##If you want analog
#thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)

while True:
    R = 10000
    Do = pin.value
    dR = R/(2**16/Do-1)
    #dR = (2**16)/Do*(R) - R #<-It might be this. Double check your math
    B = 3950
    To = 25 + 273.1
    invT = (1/To) + math.log(dR/10000)/B
    T = 1/invT - 273.1
    #temp = thermistor.value #if you want analog
    print((Do,T)) #Remember this command prints Digital Output and Temperature
    #but you probably want to uncomment the line below
    #print((time.monotonic(),T))
    
    time.sleep(0.5)
