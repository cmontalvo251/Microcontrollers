import numpy as np
import matplotlib.pyplot as plt

#Opening the File
file = open('Timestamp_Mode.txt')

first_row = None
second_row = None

t = []
x = []
y = []
z = []
T = []

##Looping through file
for line in file:
    ##Printing the list for every row
    row = line.split(',')
    #print('Raw Row = ',row)
    ##Checking to see if the row is valid
    if len(row) == 5:
        ##Getting our time flag
        time = row[2]
        ##If time is valid continue
        if (len(time)) != 0:
            ##convert to a double
            time = np.double(time)
            ##Check to see if this is a first row
            #or a second row
            if np.round(time) == time:
                second_row = row
                if first_row is not None:
                    ##Unpack the data
                    t.append(np.double(first_row[2]))
                    x.append(np.double(first_row[3]))
                    y.append(np.double(first_row[4]))
                    first_row = None
                    z.append(np.double(second_row[3]))
                    T.append(np.double(second_row[4]))
                    #print('Valid Packet Received')
                    #print(first_row)
                    #print(second_row)
                    #print('--------------')
                    second_row = None
                    
            else:
                first_row = row    

t = np.array(t)
x = np.array(x)
y = np.array(y)
z = np.array(z)
T = np.array(T)

plt.plot(t,x)
plt.plot(t,y)
plt.plot(t,z)
plt.figure()
plt.plot(t,T)
plt.show()
    