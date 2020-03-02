import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('Test_Data.txt')

time = data[:,0]
light = data[:,1]
temp = data[:,2]

plt.figure()
plt.plot(time,light)
plt.xlabel('Time (sec)')
plt.ylabel('Light (lux)')
plt.grid()

plt.figure()
plt.plot(time,temp)
plt.xlabel('Time (sec)')
plt.ylabel('Temp (F)')
plt.grid()

plt.show()