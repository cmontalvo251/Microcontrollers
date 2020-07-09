import time
import board
import busio
import math
import digitalio
import adafruit_lis3dh
import pulseio

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

# Initialize PWM output for the servo (on pin A2):
servo = pulseio.PWMOut(board.A2, frequency=50)

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

while True:
    #Get Accelerometer
    x,y,z = lis3dh.acceleration
    #Get theta
    theta = math.atan2(x,z)*180/3.141592654
    #Get elevator angle
    de = -theta
    #Get servo angle
    s = de + 90.0
    #Saturation Filter
    if s < 0:
        s = 0.
    if s > 180:
        s = 180.
    #Get pulse width
    pulse_ms = 0.6 + 0.01*s
    #Get duty cycle
    duty_cycle = servo_duty_cycle(pulse_ms)
    #Actuate Servo
    servo.duty_cycle = duty_cycle
    print(theta,de,s,pulse_ms)
    time.sleep(0.1)