#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import sys
from gps import *
from pdf import *
import mymath as M
sys.path.append('../FASTPitot/')
from pitot import create_PTH_plots
#from fileinput import filename

def get_anemometer_data(inputfilename,sigma,debugmode=0,pp=None,NOGPS=0):
    #Read data from file
    print('Loading File...')

    anemometerdata = open(inputfilename)

    time_arduino = []
    gps_data = []
    sensorvalue = []
    windspeed = []
    lat_vec = []
    lon_vec = []
    time_gps = []
    alt_vec = []
    alt_old = 0
    temp_vec = []
    pressure_vec = []
    humidity_vec = []
    time_hr = -1

    # gps_data needs to be like this
    # lat_vec_np = data[0]
    # lon_vec_np = data[1]
    # time_vec_np = data[2]
    # x_vec_np = data[3]
    # y_vec_np = data[4]
    # alt_vec_np = data[5]
    
    lenfile = 0

    for line in anemometerdata:
        lenfile+=1
        #Make sure the length of the row is valid
        if len(line) > 2:
            row = line.split(' ')

            #Need to add one more check for time
            time_split = row[0].split(':')
            #If the hour is zero and time_hr hasn't been initialized 
            #This makes sure that the time is not zero, you haven't initialized time_hr
            #and you have a valid GPS loc. This should only run once
            if (np.float(time_split[0]) != 0 and time_hr == -1):
                if abs(np.float(row[2])) > 28 and abs(np.float(row[2])) < 32:
                    if abs(np.float(row[3])) > 86 and abs(np.float(row[3])) < 90:
                        if abs(np.float(row[6])) < 1000:
                            #Otherwise save the current time
                            time_hr = np.float(time_split[0])
                            time_new = NMEA_TIME(row[0],'hrs')
                            #Save Previous values of lat lon and altitude
                            lat_new = np.float(row[2])
                            lon_new = np.float(row[3])
                            alt_new = np.float(row[6])
                            print("Hour of Day = ",time_hr)
                            print("GPS Coordinate = ",lat_new," ",lon_new)
                            print("Altitude = ",alt_new)

            #Don't grab data until we have a GPS loc
            #The above line of code makes sure we don't grab any data until we have a loc. 
            #Technically this is just as bad as the iMet but this line of code actually just says
            #once you get a lock. Take data. Furthermore the sensor itself is always sensing data we just aren't plotting
            #data until we get a lock. The lines that follow check to make sure we have a lock. If we don't it might mean we just 
            #lost the loc so we will just assume that the previous data point is valid
            
            #Problem is what if we never get a lock. What if we're doing an experiment in the lab and
            #we just want to plot some data and see what's going on?
            if NOGPS:
                time_new = 0
                time_hr = 0
                lat_new = 0
                lon_new = 0
                alt_new = 0

            #If this is tru it means we had a GPS loc at one point or another
            if time_hr != -1 or NOGPS:
                #Check and make sure our time is valid otherwise use previous value?
                if abs(np.float(time_split[0])-time_new) < 2: #Crap what if we rolled over on the hr?
                    time_new = NMEA_TIME(row[0],'hrs')
                time_gps.append(time_new)

                #Check and see if our GPS Latitude is valid
                if abs(np.float(row[2])) > 28 and abs(np.float(row[2])) < 32: 
                    #We still have a GPS loc so update the value
                    lat_new = np.float(row[2])
                lat_vec.append(lat_new)

                #Check and see if our GPS longitude is valid
                if np.float(row[3]) > -90 and np.float(row[3]) < -86: 
                    #We still have a GPS loc so update the value
                    lon_new = np.float(row[3])
                lon_vec.append(lon_new)

                #Check and see if our altitude is still valid
                if np.float(row[6]) > 5 and np.float(row[6]) < 1000: 
                    alt_new = np.float(row[6])
                alt_vec.append(alt_new)

                #Always append arduino time
                time_arduino.append(np.float(row[1]))

                #and anemometer probe data
                sensorvalue.append(np.float(row[7]))
                windspeed.append(np.float(row[8]))

                if len(row) > 13:
                    temp = np.float(row[9])
                    pressure = np.float(row[10])
                    humidity = np.float(row[11])
                    temp_vec.append(temp)
                    pressure_vec.append(pressure)
                    humidity_vec.append(humidity)


    #Fix Time_arduino in the event of double files
    time_arduino = M.unwrap(time_arduino)

    #Convert everything to np lists
    lat_vec_np = np.array(lat_vec)
    lon_vec_np = np.array(lon_vec)
    time_gps_np = np.array(time_gps)
    alt_vec_np = np.array(alt_vec)

    #Check for GPS
    if len(time_gps_np) == 0:
        #Run again except set GPS to none
        data_anem = get_anemometer_data(inputfilename,sigma,debugmode,pp,1)
        return data_anem

    #Get arduino and gps delta
    dt_gps = (time_gps_np[-1] - time_gps_np[0])*3600 #Assume gps is in seconds
    dt_arduino = time_arduino[-1] - time_arduino[0]

    #Now we must scale the arduino time to match gps_time

    #Set anemometer probe time to arduino time for now
    time_np = np.array(time_arduino)
    if NOGPS == 0:
        time_np = (time_np-time_np[0])*dt_gps/dt_arduino
    sensorvalue_np = np.array(sensorvalue)
    wind_np = np.array(windspeed)

    #Now recheck the arduino time_np
    #dt_arduino_new = time_np[-1]-time_np[0]
    #print dt_gps,dt_arduino,dt_arduino_new

    #With the anemometer time shifted it's possible now to compute total gps+arduino time
    time_arduino_hr = time_np/3600
    time_arduino_hr_offset_gps = time_arduino_hr + time_gps_np[0]
    tot_time = []
    gps_offset = 0
    tot_time1 = time_gps_np[0]
    gps_old = time_gps_np[0]
    lastGPStime = 0

    for x in range(0,len(time_gps_np)):
        #Reset Arduino clock
        if time_gps_np[x] != gps_old:
            lastGPStime = time_arduino_hr[x]
            gps_old = time_gps_np[x]

        new_time = time_gps_np[x] + time_arduino_hr[x] - lastGPStime
 
        if len(tot_time) > 0:
            if new_time < tot_time[-1]:
                lastGPStime = lastGPStime - tot_time[-1] + new_time
                new_time = time_gps_np[x] + time_arduino_hr[x] - lastGPStime
            
        tot_time.append(new_time)

    tot_time_np = np.array(tot_time)

    if debugmode == 1:
        plt.figure()
        plt.plot(time_gps_np,color='blue',label="GPS Time")
        plt.plot(time_arduino_hr_offset_gps,color='red',label="Scaled Arduino + GPS Offset Time")
        plt.plot(tot_time_np,color='green',label="GPS+Arduino Time")
        plt.xlabel('Row Number')
        plt.ylabel('Time (hrs)')
        plt.legend(loc=2)
        plt.grid()
        pp.savefig()

    tot_time_hr = tot_time_np
    tot_time_sec = tot_time_np*3600
    tot_time_sec_zero = tot_time_sec - tot_time_sec[0]

    #Convert lat and lon to Cartesian Coordinates
    if len(lat_vec_np) > 1:
        origin = [lat_vec_np[0],lon_vec_np[0]]
    else:
        origin = [0,0]

    lat_lon = [lat_vec_np,lon_vec_np]
    xy = convertLATLON(lat_lon,origin)
    x_vec_np = xy[0]
    y_vec_np = xy[1]

    print('File Loaded')

    voltageConversionConstant = .004882814; ##This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
    voltageMin = .4; ## Mininum output voltage from anemometer in mV.
    windSpeedMin = 0; ## Wind speed in meters/sec corresponding to minimum voltage
    voltageMax = 2.0; ## Maximum output voltage from anemometer in mV.
    windSpeedMax = 32; ## Wind speed in meters/sec corresponding to maximum voltage

    #The new anemometer software unfortunately outputs sensor voltage rather
    #than sensorvalue so we need to do this only if the sensorvalue mean is
    #over say 30
    m_val = np.mean(sensorvalue_np)
    if m_val > 30:
        sensorvoltage = voltageConversionConstant*sensorvalue_np
    else:
        sensorvoltage = sensorvalue_np
    airspeed_ms = ((sensorvoltage-voltageMin)*windSpeedMax/(voltageMax-voltageMin))

    ##Anemometer has consistently underestimated wind speed by a small factor
    airspeed_ms = airspeed_ms*1.6*0.985 ###THIS 1.5 seems way to perfect but hey no really sure at the moment.

    #Check for negative airspeed
    x = 0
    for airspeed in airspeed_ms:
        if airspeed < 0:
            airspeed_ms[x] = 0
        x+=1
            

    ##Run signal through a complimentary filter
    #sigma = 0.03 #Made this an input to the function so we can change it on the fly
    airspeed_ms_filtered = np.zeros(len(airspeed_ms))
    airspeed_ms_filtered[0] = airspeed_ms[0]
    for idx in range(0,len(airspeed_ms)-1):
        airspeed_ms_filtered[idx+1] = (1-sigma)*airspeed_ms_filtered[idx] + sigma*airspeed_ms[idx]

    print('Filter Complete')

    #RAW SIGNAL
    print('DebugMode = ',debugmode)
    if debugmode == 1:
        print('Plotting Raw Voltage')
        plt.figure()
        plt.plot(tot_time_sec_zero,sensorvoltage)
        plt.xlabel('Time (sec)')
        plt.ylabel('Raw Voltage (V) ')
        plt.grid()
        pp.savefig()
        
    #Return Data
    # gps_data needs to be like this
    # lat_vec_np = data[0]
    # lon_vec_np = data[1]
    # time_vec_np = data[2]
    # x_vec_np = data[3]
    # y_vec_np = data[4]
    # alt_vec_np = data[5]
    directory_gps = "GPS Data \n" + "Latitude - 0 \n" + "Longitude - 1 \n" + "tot_time_hr - 2 \n" + "x_vec_np - 3 \n" + "y_vec_np - 4 \n" + "alt_vec_np - 5 \n" + "Note that GPS Start = data[2][0] and GPS End = data[2][-1] \n"
    data_gps = [lat_vec_np,lon_vec_np,tot_time_hr,x_vec_np,y_vec_np,alt_vec_np,directory_gps]
    # Anemometer data is like this
    # tot_time_sec_zero = data[0]
    # windspeed = data[1]
    # airspeed_ms = data[2]
    # airspeed_ms_filtered = data[3]
    directory_anemometer = "Anemometer Data \n" + "tot_time_sec_zero - 0 \n" + "Windspeed Arduino - 1 \n" + "airspeed_ms - 2 \n" + "airspeed_ms_filtered - 3" + "raw voltage - 4 \n"
    data_anemometer = [tot_time_sec_zero,wind_np,airspeed_ms,airspeed_ms_filtered,sensorvoltage,directory_anemometer]

    #Convert Pressure to Altitude
    pressure_np = np.array(pressure_vec)
    if len(pressure_np) > 0:
        pressure_altitude_np = 44330.0 * (1.0 - pow(pressure_np/pressure_vec[0], 0.1903));
    else:
        pressure_altitude_np = []
    PTH_directory = "Temperature/Pressure/Humidity \n" + "tot_time_sec_zero - 0 \n" + "Temperature (C) - 1 \n" + "Pressure (hPa) - 2 \n" + "Humidity (%) - 3 \n" + "Pressure Altitude (m) \n"
    PTH_data = [tot_time_sec_zero,np.array(temp_vec),pressure_np,np.array(humidity_vec),pressure_altitude_np,PTH_directory]

    #Create a directory of everything
    directory_all = "FASTAnemometer Sensor Data \n" + "GPS Data - 0 \n" + "Anemometer Data - 1 \n" + "Temperature\Pressure\Humidity Data - 2 \n"

    #Combine into data ball
    data = [data_gps,data_anemometer,PTH_data,directory_all]

    return data

def create_anemometer_plots(data_anemometer,pp):
    #Create Anemometer Plots
    tot_time_sec_zero = data_anemometer[0]
    windspeed = data_anemometer[1]
    airspeed_ms = data_anemometer[2]
    airspeed_ms_filtered = data_anemometer[3]

    ##PROCESSED DATA
    plt.figure()
    plt.plot(tot_time_sec_zero,windspeed,label='Arduino Airspeed')
    plt.plot(tot_time_sec_zero,airspeed_ms,label='Computed Airspeed')
    plt.plot(tot_time_sec_zero,airspeed_ms_filtered,label='Filtered Airspeed')
    plt.xlabel('Time (sec)')
    plt.ylabel('Windspeed (m/s)')
    plt.legend()
    plt.grid()
    pp.savefig()
	

if __name__ == "__main__":



    print('Processing Anemometer Probe File')

    #Getting input arguments
    if len(sys.argv) < 2:
        print('Not enough input arguments. Need location of file')
        sys.exit()

    if len(sys.argv) > 2:
        print(sys.argv)
        inputfilenames = [sys.argv[1],sys.argv[2]]
    else:
        inputfilenames = [sys.argv[1]]

    #inputfilename = 'Data_Files/HOME_EXPERIMENT.TXT'
    #inputfilename = 'Data_Files/GPSLOG00.TXT'
    #inputfilename = 'Data_Files/6_30_2016_Anemometer.TXT'

    #PROCESS GPS DATA - DON'T DO THIS ANYMORE
    #I don't need to do this anymore since the Arduino Parses everything for us.
    #data = gps_data(inputfilename) #this saves the data

    #If you want to plot you need to make a global
    SHOWPLOTS = 0 #1 = show plots, 0 = convert to PDF
    pp = PDF(SHOWPLOTS,plt)

    #PROCESS ANEMOMETER AND GPS DATA
    sigma = 0.01 #Closer to 0 means more filtering
    data_all = []
    for x in range(0,len(inputfilenames)):
        inputfilename = inputfilenames[x]
        data = get_anemometer_data(inputfilename,sigma,1,pp) #1 = debugmode on, if debug mode is on you need to send pp
        data_all.append(data)
        print(data[-1]) #to see directory
        data_gps = data[0]
        data_anemometer = data[1]

        #Anemometer Data - Remember you can just print data_anemometer[-1] to see where everything is
        print(data_anemometer[-1])
        # tot_time_sec_zero = data[0]
        # windspeed = data[1]
        # airspeed_ms = data[2]
        # airspeed_ms_filtered = data[3]

        #Create GPS Plots
        print(data_gps[-1])
        print("GPS Start = ",data_gps[2][0])
        print("GPS End = ",data_gps[2][-1])
        # gps_data needs to be like this
        # lat_vec_np = data[0]
        # lon_vec_np = data[1]
        # tot_time_gps = data[2]
        # x_vec_np = data[3]
        # y_vec_np = data[4]
        # alt_vec_np = data[5]
        FP_GPS_END = data_gps[2][-1]
        FP_GPS_START = data_gps[2][0]
        del_min = np.ceil((FP_GPS_END-FP_GPS_START)*60.0/10.0) #number of minutes

        #This will just plot one data stream at a time
        create_gps_plots(data_gps,del_min,pp,False) #this plots it

        #Make this 1 data stream as well
        create_anemometer_plots(data_anemometer,pp)

    figureW = plt.figure()
    pltW = figureW.add_subplot(1,1,1)

    data_PTH_all = []
    data_gps_all = []
    for x in range(0,len(inputfilenames)):
        data_x = data_all[x]

        data_gps = data_x[0]
        data_anemometer = data_x[1]
        data_PTH = data_x[2]

        data_PTH_all.append(data_PTH)
        data_gps_all.append(data_gps)
        
        ##Plotting windspeed as function of GPS time
        time_gps_np = data_gps[2]
        windspeed = data_anemometer[1]
        airspeed_ms_filtered = data_anemometer[3]

        time_vec_HHMM,xticks = HHMM_Format(time_gps_np,del_min)

        #1 Minute averages
        avg = M.averages(time_gps_np,airspeed_ms_filtered,1)
        
        pltW.plot(time_gps_np,airspeed_ms_filtered,label='Filtered Airspeed')
        #pltW.plot(avg[0],avg[1],marker = 's',label='One Minute Averages')

        
    pltW.set_xticks(xticks) 
    pltW.set_xticklabels(time_vec_HHMM,rotation=0,ha='right',fontsize=12)
    axes = plt.gca()
    axes.set_xlim([time_gps_np[0],time_gps_np[-1]])    
    pltW.set_xlabel('Time (HH:MM)')
    pltW.set_ylabel('Windspeed, m/s')
    pltW.legend()
    
    pltW.grid()
    pp.savefig()

    #Create PTH Plots
    #let's print the PTH directory
    print(data_PTH_all[0][-1])
    create_PTH_plots(data_PTH_all,data_gps_all,pp)

        
    #Close pp
    pp.close()




    
    

