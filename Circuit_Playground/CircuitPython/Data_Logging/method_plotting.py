import numpy as np
import matplotlib.pyplot as plt

data1 = np.loadtxt('Method1.txt')
data2 = np.loadtxt('Method2.txt')
data3 = np.loadtxt('Method3.txt')

time1 = data1[:,0]
time2 = data2[:,0]
time3 = data3[:,0]

time1-=time1[0]
time2-=time2[0]
time3-=time3[0]

a1 = data1[:,1]
a2 = data2[:,1]
a3 = data3[:,1]

plt.plot(time1,a1,label='Method1')
plt.plot(time2,a2,label='Method2')
plt.plot(time3,a3,label='Method3')
plt.legend()
plt.show()
