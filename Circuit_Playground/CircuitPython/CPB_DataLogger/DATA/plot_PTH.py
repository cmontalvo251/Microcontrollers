import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('CPB_Datalog_MS8607.txt')

s = np.shape(data)
print('Shape = ',s)

t = data[:,0]
t-=t[0]

labels = ['Accel X (m/s^2)','Accel Y (m/s^2)','Accel Z (m/s^2)','Pressure (Pa)','Relative Humidity (%)','Temp Ext (C)','Temp CPB (C)']

for x in range(1,s[1]):
    plt.figure()
    plt.plot(t,data[:,x])
    plt.grid()
    plt.xlabel('Time (sec)')
    plt.ylabel(labels[x-1])

##Compare Temperature
plt.figure()
plt.plot(t,data[:,6],label='External')
plt.plot(t,data[:,7],label='CPB')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Temperature (C)')
plt.legend()

plt.show()
