import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('CPB_Datalog_IMU.txt')

s = np.shape(data)
print('Shape = ',s)

t = data[:,0]
t-=t[0]

labels = ['Gx (rad/s)','Gy (rad/s)','Gz (rad/s)','Mag X (Gauss)','Mag Y (Gauss)','Mag Z (Gauss)','Accel X Ext (m/s^2)','Accel Y Ext (m/s^2)','Accel Z Ext (m/s^2)','Accel X (m/s^2)','Accel Y (m/s^2)','Accel Z (m/s^2)','Temp (C)']

for x in range(1,s[1]):
    plt.figure()
    plt.plot(t,data[:,x])
    plt.grid()
    plt.xlabel('Time (sec)')
    plt.ylabel(labels[x-1])

##Compare Acceleration
plt.figure()
plt.plot(t,data[:,7],label='External')
plt.plot(t,data[:,11],label='CPB')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Accel X (m/s^2)')
plt.legend()

plt.figure()
plt.plot(t,data[:,8],label='External')
plt.plot(t,-data[:,10],label='CPB')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Accel Y (m/s^2)')
plt.legend()

plt.figure()
plt.plot(t,data[:,9],label='External')
plt.plot(t,data[:,12],label='CPB')
plt.grid()
plt.xlabel('Time (sec)')
plt.ylabel('Accel Z (m/s^2)')
plt.legend()


plt.show()
