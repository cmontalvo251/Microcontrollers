import time
import board
import busio
import digitalio
import adafruit_lis3dh
import math
import pwmio

# Initialize PWM output for the servo (on pin A2):
servo = pwmio.PWMOut(board.A6, frequency=50)

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G
#Initialize Filter
theta_f = 0.0
#Set filter constant
s = 1.0
#Infinite While Loop
while True:
    #Measure Acceleration
    x,y,z = lis3dh.acceleration
    ##Compute angle
    theta = math.atan2(x,z)*180/3.141592654
    ##Filter Angle
    theta_f = (1-s)*theta_f + s*theta
    ##Saturation if statements
    if theta_f > 90:
        theta_f = 90
    if theta_f < -90:
        theta_f = -90
    ###Convert to Servo Angle
    servo_angle = theta_f + 90
    ##Convert servo angle to duty cylc
    pulse_ms = 0.0099*servo_angle + 0.9
    ##Send pulse to servo
    servo.duty_cycle = servo_duty_cycle(pulse_ms)
    ##Print stuff
    print((theta,theta_f,servo_angle,pulse_ms))
    ##Wait.
    time.sleep(0.1)