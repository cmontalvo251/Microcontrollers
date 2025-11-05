import numpy as np
import matplotlib.pyplot as plt

files = ['High_Light.txt']

for i in files:
    data = np.loadtxt(i)
    V = 3.3*data[:, 1]/2**16
    R = ((3.3/V)-1)*10000
    L = 500000*(R**(-0.7))
    
    mean = np.mean(L)
    dev = np.std(L)
    med = np.median(L)
    
    print('Mean of Data Set = ', mean)
    print('Standard Deviation of Data Set = ', dev)
    print('Median of Data Set = ', med)
    
    plt.figure()
    r = np.linspace(np.min(L), np.max(L), 10)
    plt.hist(L, bins=r)
    plt.title('Histogram of Data (' + str(i) + ')')
    plt.xlabel('Light Level (Lux)')
    plt.ylabel('Number of Occurances')
    plt.grid(True)
    
plt.show()