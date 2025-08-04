#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import sys
import mymath as M
import plotting as P
from pdf import *
import mio as I

data_more_height = I.dlmread('datalog0_outdoor_more_height.csv')
data_less_height = I.dlmread('datalog1_outdoor_less_height.csv')
data_indoor = I.dlmread('datalog2_indoor.csv')

pp = PDF(0,plt)

##more indoor
plti = P.plottool(12,'Time (sec)','Winds (m/s)','Indoor')
plti.plot(data_indoor[:,0],data_indoor[:,2:5])
pp.savefig()

##less height
plti = P.plottool(12,'Time (sec)','Winds (m/s)','Less Height')
plti.plot(data_less_height[:,0],data_less_height[:,2:5])
pp.savefig()

##more height
plti = P.plottool(12,'Time (sec)','Winds (m/s)','More Height')
plti.plot(data_more_height[:,0],data_more_height[:,2:5])
pp.savefig()


pp.close()


