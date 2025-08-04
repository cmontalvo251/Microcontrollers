import numpy as np
import matplotlib.pyplot as plt

#This data file is nasty so let's write our own import routine
file = open('CupAnemometerDataOutside.TXT')
#What data do we want?
time = []
voltage = []
month = 12
day = 1
for line in file:
    row = line.split(' ')
    #print(row)
    #Throw out rows that don't have a full data stream
    if len(row) == 16:
        #Grab data that we care about
        t = np.float(row[1])
        v = np.float(row[7])
        #Append to vectors
        time.append(t)
        voltage.append(v)
        #When was this data taken?
        monthi = int(row[-2])
        dayi = int(row[-3])
        if month == monthi:
            if dayi != day:
                day = dayi
                print('Cup Anemometer Data Taken: ',month,'/',day)
file.close()
#Convert everything to numpy arrays
time = np.asarray(time)
voltage = np.asarray(voltage)

#Let's also grab the sonic anemometer file
file = open('UltrasonicDataOutside.TXT')
time_sonic = []
bits_speed = []
bits_angle = []
day = 1
for line in file:
    #print(line)
    row = line.split(' ')
    #print(row)
    if len(row) == 13:
        t = np.float(row[1])
        bs = np.float(row[7])
        ba = np.float(row[8])
        time_sonic.append(t)
        bits_speed.append(bs)
        bits_angle.append(ba)
        monthi = int(row[-3])
        dayi = int(row[-4])
        if month == monthi:
            if dayi != day:
                day = dayi
                print('Sonic Anemometer Data Taken: ',month,'/',day)
file.close()
time_sonic = np.asarray(time_sonic)
bits_speed = np.asarray(bits_speed)
bits_angle = np.asarray(bits_angle)

#The sonic anemometer reads bits so we have to convert bits to voltage first
f = 5.0/1023.
voltage_speed = f*bits_speed
voltage_angle = f*bits_angle

#Now that we have data let's plot it
plt.figure()
plt.plot(time,voltage,label='Cup Anemometer')
plt.plot(time_sonic,voltage_speed,label='Sonic Anemometer')
plt.xlabel('Time (sec)')
plt.ylabel('Voltage (V)')
plt.grid()
plt.legend()

##Let's now convert voltage to windspeed
#Taken from Cup Anemometer Data Sheet
windspeed = ((voltage - 0.39)*50.0/(2.0 - 0.4));
#Need fitting equation for sonic anemometer
#Using last DIS report to convert to HZ
m = 30./125.
b = 0.0
windtunnel_hz = bits_speed*m + b
#Then using Schibelius thesis to convert to windspeed
m = 13.0/30.0
b = 0.0
windspeed_sonic = windtunnel_hz*m + b
wf = 0*windspeed_sonic
wf[0] = windspeed_sonic[0]
for ctr in range(0,len(windspeed_sonic)-1):
    w = windspeed_sonic[ctr]
    wf[ctr+1] = (0.6*(w-wf[-1])+wf[-1])
    
#We can also do something similar for the angle
bits1 = 132
bits2 = 931
angle1 = 157.5
angle2 = -180
#(y2-y1) = m*(x2-x1)
m = (angle2-angle1)/(bits2-bits1)
#ystar = m*(xstar - x1) + y1
test_bits = np.linspace(132,931,1000)
test_angle = m*(test_bits - bits1) + angle1
plt.figure()
plt.plot(test_bits,test_angle)

sonic_angle = m*(bits_angle - bits1) + angle1

plt.figure()
plt.plot(time_sonic,bits_angle)

plt.figure()
plt.plot(time_sonic,sonic_angle)
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Angle (deg)')


##And plot it
plt.figure()
plt.plot(time,windspeed,label='Cup Anemometer')
#plt.plot(time_sonic,windspeed_sonic,label='Sonic Anemometer')
plt.plot(time_sonic,wf,label='Sonic Filtered')
plt.xlabel('Time (sec)')
plt.ylabel('Windspeed (m/s)')
plt.grid()
plt.legend()

plt.show()

