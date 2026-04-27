######################


import serial
import sys
import time
import os
import numpy as np
import matplotlib.pyplot as plt
class CPX:
    ##############################################################################
    def __init__(self,portname,index):
        print('Attempting to open serial port')
        self.index = index
        self.filename = 'CPX'+str(index)+'.txt'
        self.file = open(self.filename,'w')
        self.serialPort = serial.Serial(portname,115200)
        self.t = -99
        self.button = -99
        self.x = -99
        print('if no errors port opened')
        print('portname: ',portname)
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
        print("buffer:",buffer_list)
        try:
            self.t = self.convert(buffer_list[0])
            self.button = self.convert(buffer_list[1])
            self.x = self.convert(buffer_list[2])
            outstring = str(self.t) + " " + str(self.button) + " " + str(self.x) + "\n"
            self.file.write(outstring)
            self.file.flush()
        except:
            pass
        
    def printdata(self,time):
        print(time,self.t,self.button,self.x)

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
        #If using Thonny in a temp folder you need this extra copy to 
        #port everything over to a useable directory
        #os.system('cp CPX0.txt /home/carlos/CP0.txt')

##############################################################################
##############################################################################

##Try and open the CPX
addr = "/dev/ttyACM0"
cpx = CPX(addr,0)

###Read data
startTime = time.time()
while time.time() - startTime  < 10: #Take data for 10 seconds
    cpx.read()
    cpx.printdata(time.time()-startTime)

##Close the file and extra data
cpx.close()
print('Closed serial')
print('program end')
cpx.extractdata()
cpx.plot()
