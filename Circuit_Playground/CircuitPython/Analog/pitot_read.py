import time
import board
import pulseio
import analogio
import math

pin = analogio.AnalogIn(board.A2)

def getvoltage(analog):
    return analog*5.0/2**16

###Calibrate our sensor
N = 1000.0
meanvoltage = 0.0
for x in range(0,N):
    analog = pin.value
    voltage = getvoltage(analog)
    meanvoltage+=voltage
meanvoltage/=N

print('Mean Voltage = ',meanvoltage)
time.sleep(2)

###if dkPa = 1.0 -> dV = 1.0

# Main loop will run forever moving between 1.0 and 2.0 mS long pulses:
velf = 0.0
f = 0.9
while True:
    analog = pin.value
    voltage = getvoltage(analog)
    dV = voltage - meanvoltage
    ##Trend line that came from calibration
    dkPa = dV
    dPa = 1000*dkPa
    rho = 1.225
    if dPa < 0:
        vel = -math.sqrt(-2*dPa/rho)
    else:
        vel = math.sqrt(2*dPa/rho)
    velf = velf*f + (1-f)*vel
    #print((analog,voltage,dV))
    print((time.monotonic(),vel,velf))
    time.sleep(0.1)