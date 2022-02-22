import numpy as np
import matplotlib.pyplot as plt
import time

#Opening the File
file = open('Brignac_Bluetooth.txt')

t = []
x = []
y = []
z = []
T = []

def append_packet(packet,row):
    print('Merging Packet == ',packet)
    print('To this new row == ',row)
    ##First check to see if packet is empty
    if len(packet) > 0:
        #Then I want packet[-1] and row[2] to merge
        #but only if the len(row) is greater than 3
        if len(row)>3:
            packet[-1]+=row[2]
            for x in range(3,len(row)):
                packet.append(row[x])
    else:
        ##Otherwise the row becomes the packet
        packet = row
    print('New Packet = ',packet)
    return packet

packet = []
##Looping through file
for line in file:
    print('------------------------------------')
    #time.sleep(1.0)
    ##Split the row by commas
    row = line.split(',')
    ##Get rid of all the \n's
    for i in range(0,len(row)):
        if '\n' in row[i]:
            row[i] = row[i].replace('\n','')
    print('Raw Row = ',row)
    third_var = row[2]
    if third_var == 'A':
        ##If this is true it means we just got a new packet
        print("New Packet Detected")
        print('PACKET = ',packet)
        ##So we append everything into our array
        if len(packet) > 1:
            t.append(np.double(packet[3].replace('\n','')))
            x.append(np.double(packet[4].replace('\n','')))
            y.append(np.double(packet[5].replace('\n','')))
            z.append(np.double(packet[6].replace('\n','')))
            T.append(np.double(packet[7].replace('\n','')))
            #Then set the row to the packet
            packet = []
            packet = row
        else:
            ##if the delta time is larger it means that this must
            ##be a continuation of the previous packet
            packet = append_packet(packet,row)
    else:
        packet = append_packet(packet,row)
    

t = np.array(t)
x = np.array(x)
y = np.array(y)
z = np.array(z)
T = np.array(T)

plt.plot(t,x,label='X')
plt.plot(t,y,label='Y')
plt.plot(t,z,label='Z')
plt.xlabel('Time (sec)')
plt.ylabel('Acceleration (m/s^2)')
plt.grid()
plt.legend()
plt.figure()
plt.plot(t,T)
plt.xlabel('Time (sec)')
plt.ylabel('Temperature (C)')
plt.grid()
plt.show()
    
