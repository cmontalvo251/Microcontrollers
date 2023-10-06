import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('Launch_Data_File.txt',delimiter=' ')

time = data[:,0]

pressure_pascals = data[:,4]

pressure_sea_level = 1010.0 #you must input the pressure at sea-level
altitude = (1.0-((pressure_pascals/pressure_sea_level)**(1.0/5.25588)))/(2.2557*(10**-5.0))


plt.plot(time,altitude)
plt.show()


