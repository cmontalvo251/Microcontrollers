import numpy as np
import matplotlib.pyplot as plt
import time

#Opening the File
file = open('Phone_Bluetooth_Data.txt')

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
time_prev = 10.821 #Grab this from the second row 3rd element
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
    #Grab the third variable. It's supposed to be time
    time_flag = row[2]
    ##If the time_flag is data we need to skip
    if time_flag != 'Data':
        ##If the time_flag > 0 it means this is a new row potentially
        if len(time_flag) > 0:
            ##We then check if we've received a full row of 5
            if len(row) >= 5:
                ##Once we know the time flag is greater than zero and this
                ##is a full row of 5 vars we can convert the time_flag to a
                #double
                time_flag = np.double(time_flag)
                ##What we need to do now is check and see if the time variable is only
                ##about 1 second larger than the previous value
                delta_time = time_flag - time_prev
                print('Time Flag = ',time_flag)
                print('Time Prev = ',time_prev)
                if delta_time < 1 and delta_time > 0:
                    ##If this is true it means we just got a new packet
                    print("New Packet Detected")
                    print('PACKET = ',packet)
                    ##So we append everything into our array
                    if len(packet) > 1:
                        t.append(np.double(packet[2].replace('\n','')))
                        x.append(np.double(packet[3].replace('\n','')))
                        y.append(np.double(packet[4].replace('\n','')))
                        z.append(np.double(packet[5].replace('\n','')))
                        T.append(np.double(packet[6].replace('\n','')))
                    #Then set the row to the packet
                    packet = []
                    packet = row
                    #Also reset the time variable
                    time_prev = time_flag
                else:
                    ##if the delta time is larger it means that this must
                    ##be a continuation of the previous packet
                    packet = append_packet(packet,row)
            else:
                ##If we don't receive a full row of 5 it means again this
                ##is a continuation
                packet = append_packet(packet,row)
        else:
            #If the time_flag is empty then we append the entire row
            packet = append_packet(packet,row)
    else:
        print('Skipping Data Row')
        

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
    
