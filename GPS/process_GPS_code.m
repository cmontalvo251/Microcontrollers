function process_GPS_code(filename)

disp('Processing GPS')

fid = fopen(filename);

time_vec = [];
lon_vec = [];
lat_vec = [];

if ~fid
  disp('File not found')
else
  EOF = 1;
  while EOF
    this_line = fgetl(fid);
    if length(this_line) > 0
      if this_line == -1
	EOF = 0;
      else
	%%%I need to parse each line by commas
	%%%This might be too hard for MATLAB. I'll think about it
	data_row = strsplit(this_line);
	GPSFLAG = data_row{1};
	LAT = data_row{4};
	LON = data_row{6};
	TIME = data_row{2};
	if GPSFLAG(6) == 'C' && length(data_row) > 6
	  lat_vec = [lat_vec;str2num(LAT)/100];
	  lon_vec = [lon_vec;str2num(LON)/100];
	  time_vec = [time_vec;str2num(TIME)];
	end
      end
    end
  end
end

figure()
plot(time_vec,lon_vec)
xlabel('Time')
ylabel('Longitude (W)')

figure()
plot(lat_vec,lon_vec)
xlabel('Latitude (N)')
ylabel('Longitude (W)')

%%Convert to Cartesion Coordinates
NM2FT=6076.115485560000;
FT2M=0.3048;
x_vec = (lat_vec - lat_vec(1))*60*NM2FT*FT2M; %%//North direction - Xf , meters
y_vec = (lon_vec - lon_vec(1))*60*NM2FT*FT2M*cos(lat_vec(1)*180/pi); %%//East direction - Yf, meters

figure()
plot(x_vec,y_vec,'bs')
hold on
plot(x_vec(1),y_vec(1),'^')
plot(x_vec(end),y_vec(end),'v')
xlabel('X (m)')
ylabel('Y (m)')


