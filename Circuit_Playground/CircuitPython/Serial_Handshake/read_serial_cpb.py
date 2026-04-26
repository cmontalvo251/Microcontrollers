######################THIS IS UNTESTED CODE!!!!!!


import serial
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
class CPX:
    ##############################################################################
    def __init__(self,portname,index):
        print('Attempting to open serial port')
        self.index = index
        self.filename = 'CPX'+str(index)+'.txt'
        self.file = open(self.filename,'w')
        try:
            self.serialPort = serial.Serial(portname,115200)
            print('if no errors port opened')
            print('portname: ',portname)
        except IOError:
            print('Error opening serial port')
            #print('portname: ',portname)
            sys.exit()
        self.norm = 0
    ##############################################################################
    def convert(self,input):
        input = input.strip('b')
        input = input.strip("'")
        input = input.strip('\\n')
        input = input.strip('\\r')
        return np.float64(input)
    ##############################################################################
    def read(self):
        inBuffer = self.serialPort.readline()
        instr = str(inBuffer)
        buffer_list = instr.split(' ')
        #print(self.index,":",buffer_list)
        self.t = self.convert(buffer_list[0])
        self.button = self.convert(buffer_list[1])

        #print('CPX = ',self.index,":",self.x, self.y, self.z, self.ax, self.ay, self.az,self.gx,self.gy,self.gz,'advrage = ',self.norm)

        outstring = str(self.t) + " " + str(self.button) + "\n"
        self.file.write(outstring)
        self.file.flush()

    def extractdata(self):
        ###EDIT THIS TO EXTRACT ALL DATA STREAMS #### DONE
        data = np.loadtxt(self.filename,delimiter=' ')
        self.tvec = data[:,0]
        self.buttonvec = data[:,1]
        self.tvec -= self.tvec[0]
    def plot(self):
        plt.figure()
        plt.plot(self.tvec,self.buttonvec)
        plt.grid()
        plt.xlabel('Time (sec)')
        plt.ylabel('Button Press')
        plt.show()

    ##############################################################################
    def close(self):
        self.serialPort.close()
        self.file.close()

##############################################################################
##############################################################################

##Try and open the CPX
try:
    addr = "/dev/ttyACM0"
    cpx = CPX(addr,0)
except:
    print('Error opening addr')
    sys.exit()

###Read data
startTime = time.time()
while time.time() - startTime  < 10: #Take data for 10 seconds
    print(time.time() - startTime)
    cpx.read()

##Close the file and extra data
cpx.close()
print('Closed serial')
print('program end')
cpx.extractdata()
cpx.plot()
