import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('CPB_Datalog.txt')

time = data[:,0]

U = data[:,1]
Uf = data[:,2]
U = U[time > 50]
Uf = Uf[time > 50]
time = time[time > 50]

plt.plot(time,U)
plt.plot(time,Uf)
plt.show()