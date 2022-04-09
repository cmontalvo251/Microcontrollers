import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('CPB_Datalog.txt')

time = data[:,0]

U = data[:,1]
Uf = data[:,2]
U = U[time > 50]
Uf = Uf[time > 50]
time = time[time > 50]

plt.plot(time,U,label='Raw Signal')
plt.plot(time,Uf,label='Filtered Signal')
plt.xlabel('Time (sec)')
plt.ylabel('Measured Windspeed (m/s)')
plt.grid()
plt.legend()
plt.show()