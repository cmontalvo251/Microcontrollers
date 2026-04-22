#This code will test all sorts of different thermistor code
import time
import board
import adafruit_thermistor
import montalvo_thermistor #this is a custom module that is basically the adafruit_thermistor but slightly modified
import analogio
import math

##If we're using the adafruit_thermistor module. All we need to do is kick off the module below
#thermistor = adafruit_thermistor.Thermistor(board.A9, 10000, 10000, 25, 3950)
#In this case the nominal resistance is 10kO
#the series resistor is 10kO
#the nominal temperature is 25.0C
#and the beta coefficienct is 3950.

##The thermistor module can be downloaded from github but you can also write your own. I've taken the adafruit_thermistor module
#and created my own with similar parameters and usage
#Note you can't use both at the same time
#monte_thermistor = montalvo_thermistor.Thermistor(board.A9,10000,10000,25,3950)

#If you don't want to use a module and you want to directly see where the analog pin value is
#converted from Digital Output to voltage and then to resistance you can just read the raw analog 
#signal. To do that use the analogio module
pin = analogio.AnalogIn(board.A9) ##If you want analog

while True:
    ##Using the thermistor module, we do is just run this block of code
    #Tmodule = thermistor.temperature
    #print((Tmodule,))
    #Or this one if we're using my custom made software
    #Tmonte = monte_thermistor.temperature
    #print((Tmonte,))
    #Or this one if you're reading the raw analog signal
    Do = pin.value
    #From here you can easily convert the Digital output to voltage
    Vm = 3.3*Do/2**16
    #Of course Vth is easily obtained from Kirchoffs Voltage law
    Vth = 3.3-Vm
    #You can then use the voltage divider rule to solve for the thermistor resistance
    #noting that this thermistor is connected to the 3.3V side again where the series resistor is 10000
    Rth = 10000*(3.3/Vm-1)
    ##And finally you can compute the temperature in celsius
    B = 3950
    To = 25 + 273.1
    invT = (1/To) + math.log(Rth/10000)/B
    T = 1/invT - 273.1
    ##Here is where I print everything
    #First column is time, second column is digital output, third column is voltage across the thermistor, 4th column is resistance
    #and last column is the temperature in Celsius
    #You're welcome for getting all of this to work
    print((time.monotonic(),Do,Vth,Rth,T))
    time.sleep(0.5)
