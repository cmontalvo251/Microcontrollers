import time
import board
import busio
import digitalio
import adafruit_lis3dh ##On board accelerometer
import math
import pulseio
import neopixel
import adafruit_mpu6050 ##off board MPU6050

##Initialize MPU6050
i2c_ = busio.I2C(board.SCL, board.SDA)
mpu = adafruit_mpu6050.MPU6050(i2c_)

# Initialize PWM output for the servo (on pin A2):
servo = pulseio.PWMOut(board.A2, frequency=50)

##Accelerometer is hooked up to SDA/SCL which is I2C or just some kind of protocol
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

#Initialize Neopixels
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0,auto_write=True)

N = 100
gz_a = 0
ax_a = 0
print('Calibrating...')
print('DO NOT MOVE!!!')
for i in range(0,N):
    ax,ay,az = lis3dh.acceleration
    ax_a += ax
    gx,gy,gz = mpu.gyro
    gz_a += gz
gz_a /= N
ax_a /= N

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

while True:
    #Accelerometer - H portion of Control Block Diagram
    x,y,z = lis3dh.acceleration
    #xm,ym,zm = mpu.acceleration
    #print((xm,ym,zm,x,y,z))
    #If the USB port is pointed along the y-body axis
    #z is down and x is forward
    theta_deg = -math.atan((x-ax_a)/z)*180./3.14
    #Also read angular velocity
    gx,gy,gz = mpu.gyro
    #print((gx,gy,gz))
    theta_dot = gz - gz_a #this is in degrees per second

    #Command - This is the reference command
    theta_command = 0*3.14/180.
    theta_dot_command = 0.0

    #Summing block
    error = theta_command - theta_deg
    error_dot = theta_dot_command - theta_dot

    #Control Block - C
    #Proportional gain
    kp = 0*-0.5 #start small!!!!
    kd = 0*-0.2
    elevator_command = kp*error + kd*error_dot

    #we need to make sure we dont break anything
    #so we need a saturation block
    elev_angle_min = -30.0 #x1
    elev_angle_max = 20.0 #x2
    if elevator_command > elev_angle_max:
        elevator_command = elev_angle_max
    if elevator_command < elev_angle_min:
        elevator_command = elev_angle_min

    #Unfortunately the command from the control block is
    #in degrees and we need the signal to be in microseconds
    #so we need to generate an equation where degrees are the input
    #and microseconds is the output. We will use a super fancy linear
    #equation to do this
    duty_min = 0.6 #y1
    duty_max = 2.4 #y2
    elev_angle_duty_min = -60.0 #x1
    elev_angle_duty_max = 60.0 #x2
    slope = (duty_max - duty_min) / (elev_angle_duty_max - elev_angle_duty_min) # (y2 - y1) / (x2-x1
    duty = slope*(elevator_command - elev_angle_min) + duty_min
    #duty = 1.0

    #Send signal to servo - this is the U command to the plant
    duty_cycle = servo_duty_cycle(duty)
    servo.duty_cycle = duty_cycle

    #Just for fun let's get some lights to move
    #Get forward and rear lights
    #theta = 90, fl = 0
    #theta = -90, fl = 255
    #slope = -255/180
    m = 100.
    r = 30.
    fl = int(-m/(2*r)*(theta_deg-r))
    if fl < 0:
        fl = 0
    if fl > 255:
        fl = 255
    #theta = 90, rl = 255
    #theta = -90, rl = 0
    #slope = 255/180
    rl = int(m/(2*r)*(theta_deg-r) + m)
    if rl < 0:
        rl = 0
    if rl > 255:
        rl = 255

    for i in range(0,5):
        pixels[i] = (fl,fl,fl)
    for i in range(5,10):
        pixels[i] = (rl,rl,rl)

    #Print everything
    print((theta_deg,theta_dot,error,elevator_command,duty,fl,rl))
    time.sleep(0.1)