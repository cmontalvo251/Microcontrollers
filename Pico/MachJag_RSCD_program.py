### MACH JAG FLIGHT RECORDER 
## ORIGINAL EDIT: TYLER JONES FALL 2021
## Raspberry Pi Pico 
## Adafruit LSM6DS33+LIS3MDL IMU & Magnometer combination breakout board
## Adafruit MPRLS Ported Pressure Sensor Breakout

### MODULES
import time
import math
import board
import busio
import digitalio
import microcontroller
import adafruit_mprls #for pressure sensore from adafruit library
import adafruit_lsm6ds #for IMU from adafruit library
from adafruit_lsm6ds.lsm6ds33 import LSM6DS33
from ulab import numpy as np


### RED EXTERNAL LED
redLED = digitalio.DigitalInOut(board.GP14)
redLED.direction = digitalio.Direction.OUTPUT

### BLUE EXTERNAL LED
blueLED = digitalio.DigitalInOut(board.GP17)
blueLED.direction = digitalio.Direction.OUTPUT

### I2C1 PIN OUT
i2c1 = busio.I2C(scl=board.GP27, sda=board.GP26, frequency=400000)

### SENSORS
## INERTIAL MEASUREMENT UNIT (LSM6DS33)
imu = LSM6DS33(i2c1)
imu.range = adafruit_lsm6ds.AccelRange.RANGE_16G 
accel_x, accel_y, accel_z = imu.acceleration #raw values
accel_xf = accel_x #'xf', 'yf', 'zf' for filter
accel_yf = accel_y
accel_zf = accel_z

## PRESSURE SENSOR (MPRLS)
mpr = adafruit_mprls.MPRLS(i2c1, psi_min=0, psi_max=25)

### INITIALIZATIONS
redLED.value = True
print('INITIALIZING...n\DO NOT MOVE!!!')

## PRESSURE
global P_int_avg, Psl, beta, h_int, h_max
P_int = []
for i in range(0,200,1):
        P = mpr.pressure*100 #sensor outputs in hPa, x100 conversion to Pa
        P_int.append(P)
        time.sleep(0.03)
P_int_avg = sum(P_int)/((len(P_int)))
print('Average Initial Pressure = ', P_int_avg, 'Pa')
Psl = P_int_avg
Pfiltered = P_int_avg

## ALTITUDE
# BETA: beta = (grav*MolarMass)/(Tsl*R)
g = 9.80665 #m/s^2
M = 0.0289644 #kg/mol
R = 8.3144598 #J/(mol*K)
LaunchTemperature = 15 #C *** ENTER TEMPURATURE AT LAUNCH DAY ***
Tsl = 273.15 + LaunchTemperature
beta = (g * M) / (Tsl * R)
#print(beta)

h_max = -1e20
h_int = (-1*math.log(P_int_avg/Psl))/beta
print('Initial Altidue = ', abs(h_int), 'm')


## PITCH AND YAW 
pitch = 0.0 # '0.0' initialization values
yaw = 0.0
pitch_i = 0.0
yaw_i = 0.0
q_bias = 0.0
p_bias = 0.0
for x in range(0,200,1): # loop to remove bias from gyro
    gyro_x, gyro_y, gyro_z = imu.gyro
    q_bias += gyro_x
    p_bias += gyro_y
q_bias/=200.0
p_bias/=200.0

print('Initialization Completed')
redLED.value = False

### PARACHUTE DEPLOYMENT FUNCTION
chute_trigger = digitalio.DigitalInOut(board.GP5)
chute_trigger.direction = digitalio.Direction.OUTPUT
trigger_angle = 75.0 #degrees
DEPLOY = 0
def chute_eject():
    chute_trigger.value = True
    print(time.monotonic(), '***PARACHUTE DEPLOYED***')
    time.sleep(1.3) # time igniter will be hot
    chute_trigger.value = False
    return

### DATALOGGING MODE
write_pin = digitalio.DigitalInOut(board.GP0)
write_pin.direction = digitalio.Direction.INPUT
write_pin.pull = digitalio.Pull.UP
filename = 'data.txt'

#Confirmation of datalogging 
if not write_pin.value:
    print('DATALOGGING COMMENCED')
    file = open(filename,'a')

else:
    print('***DATA NOT RECORDING***')
    redLED.value = True # redLED on = not datalogging

time.sleep(3.0)

start_time = time.monotonic()
print('Start Time = ',start_time)
prev_time = 0.0

while True:
    #time recording
    time_since_start = time.monotonic() - start_time
    dt = time_since_start - prev_time
    prev_time = time_since_start    
    
    #raw acceleration    
    accel_x, accel_y, accel_z = imu.acceleration
    #filtered acceleration
    r = 0.77
    accel_xf = (1-r)*accel_x + accel_xf*r
    accel_yf = (1-r)*accel_y + accel_yf*r
    accel_zf = (1-r)*accel_z + accel_zf*r
    #pitch & yaw from acceleration readings
    pitch_a = math.atan2(accel_xf,accel_zf)*180/math.pi
    yaw_a = math.atan2(accel_yf,accel_zf)*180/math.pi
    
    #gyro
    gyro_x, gyro_y, gyro_z = imu.gyro
    gyro_x-=q_bias
    gyro_y-=p_bias
    #pitch & yaw from gyro readings
    pitch_i -= gyro_y*(180.0/math.pi)*dt 
    yaw_i += gyro_x*(180.0/math.pi)*dt
    
    #filtered pitch & yaw
    a = 0.2
    pitch_i = (1-a)*pitch_i + a*pitch_a
    yaw_i = (1-a)*yaw_i + a*yaw_a

    pitch = pitch_i
    yaw = yaw_i

    #raw pressure
    Pnew = mpr.pressure*100 #hPa x100 conversion to Pa
    #filtered pressure
    s = 0.5
    Pfiltered = (1-s)*Pnew + Pfiltered*s
    
    #altitude
    h = (-1*math.log(Pfiltered/Psl))/beta
    delta_h = h - h_int
    if delta_h > h_max:
        h_max = delta_h

    ##PARACHUTE EJECTION
    #nose over function
    if abs(pitch) > trigger_angle and DEPLOY == 0:
        chute_eject()
        DEPLOY = 1

    if abs(yaw) > trigger_angle and DEPLOY == 0:
        chute_eject()
        DEPLOY =1

    #free fall function
    if (h_max - delta_h)>100 and DEPLOY == 0: # negative distance from apogee 
        chute_eject()
        DEPLOY = 1


    if not write_pin.value:
        blueLED.value = True #blue LED on confirms data is being logged
        output = str(time_since_start) + " " + str(float(accel_x)) + " " + str(float(accel_y)) + " " + str(float(accel_z)) + " " + str(float(accel_xf)) + " " + str(float(accel_yf)) + " " + str(float(accel_zf)) + " " + str(float(pitch)) + " " + str(float(yaw)) + " " + str(float(delta_h)) + " " + str(float(Pfiltered)) + " " + str(float(DEPLOY)) + " " + str('\n')
        file.write(output)
        file.flush()
        if time_since_start > 900: #closing file after 15min
            file.close()
            redLED.value = True
            time.sleep(1000)
    else:
        pass

    time.sleep(0.10) #logging data to Pico faster than 0.1s causes malfunction
