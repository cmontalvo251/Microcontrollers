#!/usr/bin/python

from gps import *

filename = 'GPS_Files/GPSFRITZ.TXT'
data = gps_data(filename)
create_gps_plots(data)

##This doesn't plot prox data - oh. poop.
