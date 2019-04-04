clear
clc
close all

fid = fopen('GPS_Files/GPSFRITZ.TXT');
fidout = fopen('GPSOUT.TXT','wb');

data_all = [];

if ~fid
    disp('File not found');
else
    EOF = 1;
    while EOF
       this_line = fgetl(fid);
       if length(this_line) > 0
           if this_line == -1
               EOF = 0;
           end
           if this_line(1) == 'o'
               this_line(1:2) = [];
               %disp(this_line)
               data_row = str2num(this_line);
               data_all = [data_all;data_row];
           end
	   if this_line(1) == '$'
	     fprintf(fidout,this_line);
	     fprintf(fidout,'\n');
	   end
       end
    end
end

time = data_all(:,1)./1000;
right_raw = data_all(:,3);
left_raw = data_all(:,4);
front_raw = data_all(:,2);

%%%Run all sensors through a filter to get rid of noise
right = moving_average(right_raw,0.9*0);
left = moving_average(left_raw,0.95*0);
front = moving_average(front_raw,0.99*0);

%%%Ignore the first 20 seconds
t20 = find(time > 20,1);
time = time(t20:end);
right = right(t20:end);
left = left(t20:end);
front = front(t20:end);

%%%Normalize sensors
right = right./max(right);
left = left./max(left);
front = front./max(front);

%%%Detect Pushing
right_thresh = mean(right);
r = right < right_thresh;
right_push = right(r);
left_thresh = mean(left);
l = left < left_thresh;
left_push = left(l);

%%%Detect foot placement
front_thresh = min(front) + 0.05;
l1 = front>front_thresh;
l2 = front<front_thresh;

figure('Name','Right')
plot(time,right,'b-')
hold on
plot(time(r),right_push,'rs')
legend('Right Sensor','Pushes')
xlabel('Time (sec)')

figure('Name','Left')
plot(time,left,'r-')
hold on
plot(time(l),left_push,'bs')
legend('Left Sensor','Pushes')
xlabel('Time (sec)')

figure('Name','Front')
plot(time,front,'g-')
hold on
plot(time(l1),front(l1),'rs')
plot(time(l2),front(l2),'bs')
legend('Front Sensor','Front Foot','Back Foot')
xlabel('Time (sec)')

process_GPS_file('GPSOUT.TXT');
