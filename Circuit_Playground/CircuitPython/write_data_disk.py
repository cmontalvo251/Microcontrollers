import time
from adafruit_circuitplayground.express import cpx

file = open('Test_Data.txt','w')

timeout = time.time() + 120
while True:
    test = 0
    x, y, z = cpx.acceleration
    Temp = cpx.temperature
    Temp = Temp * 1.8 +32
    print(time.monotonic(),x, y, z,Temp)
    output = str(time.monotonic()) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(Temp) + str('\n')
    file.write(output)
    file.flush()
    time.sleep(1)
    if time.time() > timeout:
        break
