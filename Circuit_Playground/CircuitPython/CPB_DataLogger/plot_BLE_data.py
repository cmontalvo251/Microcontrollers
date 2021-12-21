import numpy as np
import matplotlib.pyplot as plt

#Opening the File
file = open('Timestamp_Mode_w_Datalog.txt')

t = []
x = []
y = []
z = []
T = []

def append_packet(packet,row):
    ##First I want packet[-1] and row[2] to merge
    if len(row)>3:
        packet[-1]+=row[2]
        for x in range(3,len(row)):
            packet.append(row[x])
    return packet

##Looping through file
packet = []
for line in file:
    ##Printing the list for every row
    row = line.split(',')
    for i in range(0,len(row)):
        if '\n' in row[i]:
            row[i].replace('\n','')
    #print('Raw Row = ',row)
    time_flag = row[2]
    if time_flag != 'Data':
        if len(time_flag) > 0:
            if len(row) == 5:
                time_flag = np.double(time_flag)
                if time_flag > 14355 and time_flag < 14382:
                    print("New Packet Detected")
                    print(packet)
                    if len(packet) > 1:
                        t.append(np.double(packet[2].replace('\n','')))
                        x.append(np.double(packet[3].replace('\n','')))
                        y.append(np.double(packet[4].replace('\n','')))
                        z.append(np.double(packet[5].replace('\n','')))
                        T.append(np.double(packet[6].replace('\n','')))
                    packet = []
                    packet = row
                else:
                    packet = append_packet(packet,row)
            else:
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
    
