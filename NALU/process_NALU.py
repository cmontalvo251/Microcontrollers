#!/usr/bin/python

import mio as M
import gps as G
import numpy as np
import matplotlib.pyplot as plt
from pdf import *

filename = 'Some_Data/NALU022.TXT'

data = M.dlmread(filename,' ')

[numrows,numcols] = np.shape(data)

pp = PDF(0,plt)

arduinoTime = data[:,3]

ylabels = ['GPS Hour','GPS Minute','GPS Second','Arduino Time','GPS Fix','Latitude','Longitude','GPS Speed','GPS Angle','GPS Altitude','Sonar Left','Sonar Right','Euler X','Euler Y','Euler Z','Rate X','Rate Y','Rate Z','GPS Month','GPS Day']

###Plot Raw Data
for c in range(0,numcols):
    print(c)
    plt.figure()
    plt.plot(arduinoTime,data[:,c])
    plt.xlabel('Arduino Time (sec)')
    plt.ylabel(ylabels[c])
    plt.grid()
    pp.savefig()

pp.close()

# filename = 'GPS_Files/GPSFRITZ.TXT'
# data = gps_data(filename)
# create_gps_plots(data)

##This doesn't plot prox data - oh. poop.
