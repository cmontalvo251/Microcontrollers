%%%%Read NALU_XX.TXT
clear
clc
close all

%filename = 'GPS_Files/12_15_2016/PARK_TEST_QUICK.TXT';

%%%The following two files are massively aliased so I would take them with
%%%a grain of salt

filename = 'GPS_Files/12_25_2016/NALU_03.TXT'; %%%Lite-Rey Trucks
%filename = 'GPS_Files/12_27_2016/NALU_04.TXT';  %%%%Don't Trip Trucks

%%%%The files below have been updated to remove the temperature and sonar
%%%%sensors to -99. The result is I have changed the timestep to 0.05.
%%%%Check the dt plot to make sure. Just checked. It looks amazing. Yea
%%%%those sonar sensors suck and really take up time. Not sure if the temp
%%%%helped but hey it couldn't hurt.

%filename = 'GPS_Files/12_28_2016/NALU_17.TXT'; %%%Don't Trip - 1
%filename = 'GPS_Files/12_28_2016/NALU_18.TXT'; %%%Don't Trip - 2 - Towards the house

%%%The frequency of pumping from NALU_18 = 8 cycles every 4 seconds
%%%This is about 1.92 Hz - Nyquist frequency is then 4 Hz => 0.25 seconds
%%%Which means the NALU_03 and NALU_04 plots are definitely aliased
%%%although the arduino was set at 0.1 the data was taken around 0.3
%%%seconds

%%%On the contrary the NALU_18 and NALU_17 data has a nominal 0.05 seconds
%%%and a max of 0.15 which is well below 0.25 so we're good. It sucks I
%%%didn't get the Lite-Rey data but whatever.

%%%To compute frequency we have (peak to peak time = dT = peak2-peak1)
%%%cos(w*peak2) = 1,cos(w*peak1) = 1
%%%w*peak2 = n*2*pi, w*peak1 = (n+1)*2*pi
%%%w*(peak2-peak1) = w*dT = 2*pi
%%% w = 2*pi/dT (rad/s)
%%% 2*pi*f = w
%%% f = w/(2*pi) (1/s) (Hz)
%%% f = 2*pi/dT * (1/2*pi) = 1/dT
%%% ok so T = dT = period
%%% f = 1/dT
 
data = dlmread(filename);

%%%%%Remove all "BAD" GPS data
GPS_fix_col = 5;
data = remove_bad_gps(data,GPS_fix_col,0);

%%%%Combine Arduino and GPS time data
GPS_start_col = 1;
total_time_arduino_and_GPS_sec = combine_GPS_arduino_time(data,GPS_start_col,1)*60;

plottool(1,'Time Diff',12,'Time (sec)','dT (sec)');
dt = total_time_arduino_and_GPS_sec(2:end)-total_time_arduino_and_GPS_sec(1:end-1);
plot(total_time_arduino_and_GPS_sec(1:end-1),dt,'LineWidth',2)

Names={'Fix Quality','Latitude (deg)','Longitude (deg)','GPS Speed (kts)', ...
       'GPS Compass (deg)','GPS Altitude (m)','Sonar Left (cm)',...
       'Sonar Front (cm)','Sonar Right (cm)','Euler X (deg)',...
       'Pitch Angle (deg)','Roll Angle (deg)','Rate X (rad/s)','Rate Y (rad/s)',...
       'Rate Z (rad/s)','Lin Accel X (m/s^2)','Lin Accel Y (m/s^2)',...
       'Lin Accel Z (m/s^2)','Temperature (C)'};

for idx = 1:length(Names)
  plottool(1,Names{idx},12,'Time (sec)',Names{idx});
  %plot(total_time_arduino_and_GPS_min,data(:,4+idx),'LineWidth',2)
  plot(total_time_arduino_and_GPS_sec,data(:,4+idx),'LineWidth',2)
end

plottool(1,'LatvsLon',12,'Longitude (deg)','Latitude (deg)')
latitude = data(:,6);
longitude = data(:,7);
plot(longitude,latitude,'LineWidth',2)

plottool(1,'XvsY',12,'Y (m)','X (m)');
[x,y] = convertLATLON(latitude,longitude,[latitude(1),longitude(1)]);
plot(y,x,'LineWidth',2)

% GPS_speed_kts = data(:,8);
% GPS_angle = data(:,9);
% GPS_altitude = data(:,10);
% sonar_left = data(:,11);
% sonar_front = data(:,12);
% sonar_right = data(:,13);

%%%Sonar Sensor
sonar_left = data(:,11);
sonar_left(sonar_left>20) = 100;
sonar_right = data(:,13);
sonar_right(sonar_right>20) = 100;
plottool(1,'Name',12,'Time (min)','Sonar Sensor (cm)');
plot(total_time_arduino_and_GPS_min,sonar_left,'LineWidth',2)
plot(total_time_arduino_and_GPS_min,sonar_right,'r-','LineWidth',2)
legend('Left','Right')
xlim([0 1])
