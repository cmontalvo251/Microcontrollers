import time
import board
import busio
import digitalio
import adafruit_lis3dh
import math
import pwmio
import adafruit_mpu6050

i2c_ext = board.I2C()
mpu = adafruit_mpu6050.MPU6050(i2c_ext)

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

# Initialize PWM output for the servo (on pin A2):
servo = pwmio.PWMOut(board.A1, frequency=50)
servo2 = pwmio.PWMOut(board.A2,frequency=50)

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G
pulse_msf = 1.6
pulse_msf_roll = 1.6
while True:
    ###SENSOR BLOCK
    x,y,z = lis3dh.acceleration
    #Get theta
    theta = math.atan2(x,z)*180/3.141592654
    roll = -math.atan2(y,z)*180/3.141592654
    ##Rate gyro
    gx,gy,gz = mpu.gyro
    thetadot = gz

    ###SUMMING JUNCTION
    theta_ref = 0.0
    roll_ref = 0.0
    thetadot_ref = 0.0
    rolldot_ref = 0.0
    error = theta_ref - theta
    error_roll = roll_ref - roll
    errordot = thetadot_ref - thetadot
    errordot_roll = rolldot_ref - 0.0

    ###CONTROLLER -
    kp = 0.01
    kd = 0.0
    pulse_ms = kp*error + kd*errordot + 1.6
    pulse_ms_roll = kp*error_roll + kd*errordot_roll + 1.6
    if pulse_ms > 2.1:
        pulse_ms = 2.1
    if pulse_ms < 0.992:
        pulse_ms = 0.992
    if pulse_ms_roll > 2.1:
        pulse_ms_roll = 2.1
    if pulse_ms_roll < 0.992:
        pulse_ms_roll = 0.992

    s = 1.0
    pulse_msf = pulse_ms*s + (1-s)*pulse_msf
    pulse_msf_roll = pulse_ms_roll*s + (1-s)*pulse_msf_roll

    #####ACTUATOR
    #Get duty cycle
    duty_cycle = servo_duty_cycle(pulse_msf)
    duty_cycle_roll = servo_duty_cycle(pulse_msf_roll)
    #Actuate Servo
    servo.duty_cycle = duty_cycle
    servo2.duty_cycle = duty_cycle_roll

    #print((pulse_ms,pulse_msf,))
    #print((x,y,z))
    print((theta,roll))
    #print((gx,gy,gz))
    time.sleep(0.1)
