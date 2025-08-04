#!/usr/bin/python

import vicon as V
import matplotlib.pyplot as plt
import sys
from pdf import *
sys.path.append('Anemometer/')
import anemometer as ANEM
import plotting as P

#Get input arguments
if len(sys.argv) > 4:
    viconFile = sys.argv[1]
    numMarkers = int(sys.argv[2])
    anemometerFile = sys.argv[3]
    sigma = np.float(sys.argv[4])
elif len(sys.argv) == 4:
    print('viconFile, num markers and anemometer file given but need sigma')
    sys.exit()
elif len(sys.argv) == 3:
    print('viconFile and num markers given but need anemometer file and sigma')
    sys.exit()
elif len(sys.argv) == 2:
    print('viconFile given but need num markers, anemometer file and sigma')
else:
    #print(sys.argv)
    print('No viconfile found or given. Also need number of markers and anemometer file with sigma')
    sys.exit()

pp = PDF(0,plt)

##Read in Vicon Data and plot it 
vicon_data = V.get_vicon_data(viconFile,numMarkers)
#V.plot_rbd(vicon_data,pp)
time_vicon = vicon_data.t
time_vicon -= time_vicon[0]
speed_vicon = np.abs(vicon_data.ydot)

##Read in anemometer data
data_anem = ANEM.get_anemometer_data(anemometerFile,sigma,0) #1 = debugmode on, if debug mode is on you need to send pp
#print data_anem[-1]
data_anem_wind = data_anem[1]
#print data_anem_wind[-1]
time_anemometer = data_anem_wind[0]
time_anemometer -= (time_anemometer[0] + 48)
wind_anemometer = data_anem_wind[2]*1.6
raw_voltage = data_anem_wind[4]

scaled_voltage = (raw_voltage-raw_voltage[0])*100.0

##Plot Vicon Data on top of anemometer data
plti = P.plottool(12,'Time (sec)','Absolute Speed (m/s)','Vicon Data vs. Single Axis Anemometer')
plti.plot(time_vicon,speed_vicon,label='Motion Capture',color='blue')
plti.plot(time_anemometer,wind_anemometer,label='Anemometer',color='red')
#plti.plot(time_anemometer,scaled_voltage,label='Anemometer',color='red')
plti.set_xlim([0,np.max(time_vicon)])
plt.legend()
pp.savefig()

pp.close()





