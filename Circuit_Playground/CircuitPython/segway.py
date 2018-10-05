from adafruit_circuitplayground.express import cpx
import math
import time
import time
from adafruit_crickit import crickit
 
# make two variables for the motors to make code shorter to type
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2

def Compute_Theta(theta_in):
    ##Ok these are crickit frame signals
    x_total = 0
    y_total = 0
    z_total = 0
    for count in range(10):
        x, y, z = cpx.acceleration
        x_total = x_total + x
        y_total = y_total + y
        z_total = z_total + z
        time.sleep(0.001)
    x_total = x_total / 10
    y_total = y_total / 10
    z_total = z_total / 10
    
    #From here we need to rotate to the robot frame
    angle = 47.*math.pi/180.
    cos = math.cos(angle)
    sin = math.sin(angle)
    x_robot = cos*x_total + sin*y_total
    y_robot = -sin*x_total + cos*y_total
    
    #Then we can compute theta the pitch angle
    if abs(z_total) > 0.001:
        theta = math.atan(y_robot/z_total)*180./math.pi
    else:
        theta = 0.
    
    elapsed_time = 0.1
    
    thetadot = (theta_in - theta)/elapsed_time
    
    #print((x_robot,y_robot,z_total))
    return [theta,thetadot]

theta = 0
thetadot = 0
s = 0.8

while True:
    #Compute the angle of the robot
    [theta,thetadot_new] = Compute_Theta(theta)
    #Complimentary Filter
    thetadot = thetadot*s + thetadot_new*(1-s)

    #Drive the wheels based on the angle of the robot
    max_theta = 45. #Lowering this value increases the gain - make this larger to reduce oscillations
    kp = 1.0/(max_theta)
    kd = 0.5*kp
    drive = kp*theta + kd*thetadot
    if abs(drive) > 1.:
        drive = math.copysign(1.,drive)
    motor_1.throttle = -drive
    motor_2.throttle = -drive
    print((theta,thetadot))