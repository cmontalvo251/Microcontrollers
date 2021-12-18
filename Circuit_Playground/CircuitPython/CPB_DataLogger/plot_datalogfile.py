import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('CPB_Datalog.txt')

print('Shape = ',np.shape(data))

t = data[:,0]
t-=t[0]
x = data[:,1]
y = data[:,2]
z = data[:,3]
T = data[:,4]

plt.figure()
plt.plot(t,x,label='X')
plt.plot(t,y,label='Y')
plt.plot(t,z,label='Z')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Accel (m/s^2)')

plt.figure()
plt.plot(t,T)
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Temperature (C)')

plt.show()