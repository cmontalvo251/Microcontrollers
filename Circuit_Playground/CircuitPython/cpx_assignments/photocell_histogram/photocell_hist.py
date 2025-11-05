import numpy as np
import matplotlib.pyplot as plt

###IMPORT DATA
data = np.loadtxt('High_Light.txt')

###GRAB TIME
time = data[:,0]
###SHIFT TIME TO ZERO
time -= time[0]
##CONVERT DATA TO VOLTAGE
V = data[:,1]*3.3/2**16
#Convert voltage to resistance
R = ((3.3/V)-1)*10000
#Convert to Lux
voltage = 500000*(R**(-0.7))

##Throw out anything over 1000
time = time[voltage < 1000000]
voltage = voltage[voltage < 100000]

###THROW OUT BAD TIME SERIES
#voltage = voltage[time>161]
#time = time[time>161]

###THROW OUT OUTLIERS
##COMPUTE CURRENT MEAN AND DEV
mean = np.mean(voltage)
dev = np.std(voltage)
print(mean,dev)
#time = time[voltage > mean - 3*dev]
#voltage = voltage[voltage > mean - 3*dev]
#time = time[voltage < mean + 3*dev]
#voltage = voltage[voltage < mean + 3*dev]

###COMPUTE NEW MEAN,STD
mean = np.mean(voltage)
dev = np.std(voltage)
print(mean,dev)

###COMPUTE THE NORMAL DISTRIBUTION
x = np.linspace(mean-3*dev,mean+3*dev,1000)
fx = (1/(np.sqrt(2*np.pi)*dev))*np.exp(-(x-mean)**2/(2*dev**2))
maxfx = np.max(fx)

###PLOT TIME SERIES
plt.plot(time,voltage,'b*')
plt.xlabel('Time (sec')
plt.ylabel('Lux Value (L)')
plt.grid()
###PLOT HISTOGRAM
plt.figure()
bins = plt.hist(voltage,bins=np.linspace(np.min(voltage),16000,18))
bins_count = bins[0]
bins_coord = bins[1]
print(bins_count,bins_coord)
maxhist = np.max(bins_count)
plt.plot(x,fx*maxhist/maxfx)
plt.xlabel('Lux Value (L)')
plt.ylabel('Number of Occurences')
plt.grid()

plt.show()