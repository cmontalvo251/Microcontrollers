import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('ELENA_RUN.txt',delimiter=',')
time = data[:,0]
x = data[:,1]
y = data[:,2]
z = data[:,3]
norm = np.sqrt(x**2 + y**2 + z**2)
bias = norm - 9.81
START_TIME = 56
END_TIME = 94
l1 = np.where(time > START_TIME)[0][0]
l2 = np.where(time > END_TIME)[0][0]
ttruncate = time[l1:l2]
biastruncate = bias[l1:l2]
plt.figure()
plt.plot(ttruncate,biastruncate,'r-*')
STEPS = 0
RESET = False
for x in range(0,len(ttruncate)):
    if biastruncate[x] > 0 and RESET == False:
        STEPS+=1
        plt.plot(ttruncate[x],biastruncate[x],'b*')
        RESET = True
    if biastruncate[x] < 0 and RESET == True:
        RESET = False
print('STEPS = ',STEPS)
plt.show()