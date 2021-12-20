import numpy as np
import matplotlib.pyplot as plt

#Opening the File
file = open('Timestamp_Mode_w_Datalog.txt')

first_row = None
second_row = None

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
                    #print("New Packet Detected")
                    #print(packet)
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

###IMPORT THE ONBOARD DATA
data = np.loadtxt('CPB_Datalog_w_BLE.txt')
tCPB = data[:,0]
xCPB = data[:,1]
yCPB = data[:,2]
zCPB = data[:,3]
TCPB = data[:,4]

plt.plot(t,x,'--')
plt.plot(tCPB,xCPB)
plt.plot(t,y,'--')
plt.plot(tCPB,yCPB)
plt.plot(t,z,'--')
plt.plot(tCPB,zCPB)
plt.figure()
plt.plot(t,T,'--')
plt.plot(tCPB,TCPB)
plt.show()
    

