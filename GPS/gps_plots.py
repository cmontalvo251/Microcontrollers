#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
from math import *
from matplotlib.backends.backend_pdf import PdfPages
import sys
import os

#Read data from file
file = open('GPSLOG00.TXT')

lat_vec = []
lon_vec = []
time_vec = []

lenfile = 0

for line in file:
    # lenfile+=1
    #Check to make sure a line was read
    if len(line) > 0:
        #Split the line into a list
        row = line.split(',')
        #check to make sure we've got data and we're reading GPRMC
        #also check for data
        if len(row) > 2 and row[0][5]=='C' and len(row[5])>0:
            time = np.float(row[1]) #don't forget to convert to np.float
            lat = np.float(row[3])/100.0
            lon = np.float(row[5])/100.0
            print 'Time = ',time,' LAT = ',lat,' LONG = ',lon
            #Append to a vector
            time_vec.append(time)
            lat_vec.append(lat)
            lon_vec.append(lon)

#Convert lists to numpy arrays. Make sure the list contains numpy floats
lat_vec_np = np.array(lat_vec)
lon_vec_np = np.array(lon_vec)
time_vec_np = np.array(time_vec)

###SAVE FIGS
pp = PdfPages('plots.pdf')

#Lat vs time
plt.figure()
plt.plot(time_vec_np,lon_vec_np)
plt.xlabel('Time')
plt.ylabel('Longitude (W)')
plt.grid()
pp.savefig()

#Top Down View
plt.figure()
plt.plot(lat_vec_np,lon_vec_np)
plt.xlabel('Latitude (N)')
plt.ylabel('Longitude (W)')
plt.grid()
pp.savefig()

#Convert to Cartesion Coordinates
NM2FT=6076.115485560000;
FT2M=0.3048;
x_vec_np = (lat_vec_np - lat_vec_np[0])*60*NM2FT*FT2M; #%%//North direction - Xf , meters
y_vec_np = (lon_vec_np - lon_vec_np[0])*60*NM2FT*FT2M*cos(lat_vec_np[0]*180/pi); #%%//East direction - Yf, meters

plt.figure()
plt.plot(x_vec_np,y_vec_np,marker="s")
plt.plot(x_vec_np[0],y_vec_np[0],marker="^")
plt.plot(x_vec_np[-1],y_vec_np[-1],marker="v")
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
plt.grid()
pp.savefig()

plt.figure()
plt.plot(time_vec_np,y_vec_np)
plt.xlabel('Time (sec)')
plt.ylabel('Y (m)')
plt.grid()
pp.savefig()

#Run y through a derivative element
ydot_vec_np = [0]
for idx in range(0,len(y_vec_np)-1):
    ydot = (y_vec_np[idx+1]-y_vec_np[idx])/(time_vec_np[idx+1]-time_vec_np[idx])
    ydot_vec_np.append(ydot)


plt.figure()
plt.plot(time_vec_np,ydot_vec_np)
plt.xlabel('Time (sec)')
plt.ylabel('Ydot (m/s)')
plt.grid()
pp.savefig()

# #CLOSE FILE
pp.close()

# #AND THEN USE EVINCE TO OPEN PDF if on linux
if sys.platform == 'linux2':
    os.system('evince plots.pdf &')
