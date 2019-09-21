import time
from adafruit_crickit import crickit
 
print("Dual motor demo!")
 
# make two variables for the motors to make code shorter to type
motor_1 = crickit.dc_motor_1
motor_2 = crickit.dc_motor_2
 
while True:
    print("Moving Motors at Full Speed")
    motor_1.throttle = 1  # full speed forward
    motor_2.throttle = -1 # full speed backward
    time.sleep(1)
 
    print("Moving Motors at Half Speed")
    motor_1.throttle = 0.5  # half speed forward
    motor_2.throttle = -0.5 # half speed backward
    time.sleep(1)
 
    print("Stopping Motors")
    motor_1.throttle = 0  # stopped
    motor_2.throttle = 0  # also stopped
    time.sleep(1)
 
    print("Moving Motors at Half Speed")
    motor_1.throttle = -0.5  # half speed backward
    motor_2.throttle = 0.5   # half speed forward
    time.sleep(1)
 
    print("Moving Motors at Full Speed the other Way")
    motor_1.throttle = -1  # full speed backward
    motor_2.throttle = 1   # full speed forward
    time.sleep(1)
 
    print("Stopping Motors")
    motor_1.throttle = 0  # stopped
    motor_2.throttle = 0  # also stopped
    time.sleep(0.5)
    
    # and repeat!