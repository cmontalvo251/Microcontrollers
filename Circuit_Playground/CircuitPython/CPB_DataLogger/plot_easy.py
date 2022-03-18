import numpy as np
import matplotlib.pyplot as plt
data = np.loadtxt('CPB_Datalog.txt')
t = data[:,0]
x = data[:,1]
y = data[:,2]
z = data[:,3]
T = data[:,4]
plt.plot(t,x,label='x')
plt.plot(t,y,label='y')
plt.plot(t,z,label='z')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Accel (m/s^2')
plt.legend()
plt.figure()
plt.plot(t,T)
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Temp (C)')
plt.legend()
plt.show()