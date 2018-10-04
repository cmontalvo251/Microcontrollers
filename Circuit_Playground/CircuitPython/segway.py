from adafruit_circuitplayground.express import cpx
import math
import time
import time
from adafruit_crickit import crickit
 
# make two variables for the motors to make code shorter to type
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2

def Compute_Theta():
    ##Ok these are crickit frame signals - say sensor signals if you will
    x_total = 0
    y_total = 0
    z_total = 0
    #Take 10 measurements and average them
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
    theta = math.atan(y_robot/z_total)*180./math.pi
    
    #print((x_robot,y_robot,z_total))
    return theta

while True:
    #Compute the angle of the robot
    theta = Compute_Theta()
    #Drive the wheels based on the angle of the robot
    max_theta = 45. #Lowering this value increases the gain - make this larger to reduce oscillations
    kp = 1.0/(max_theta)
    drive = kp*theta
    motor_1.throttle = -drive
    motor_2.throttle = -drive
    print((theta,))