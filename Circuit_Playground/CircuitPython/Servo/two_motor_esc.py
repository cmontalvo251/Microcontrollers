#This code controls a single board ESC to control two motors
#using the CPX accelerometer
import time
import board
import busio
import digitalio
import adafruit_lis3dh
import pwmio
import math

##Accelerometer is hooked up to SDA/SCL which is I2C
i2c = busio.I2C(board.ACCELEROMETER_SCL, board.ACCELEROMETER_SDA)
_int1 = digitalio.DigitalInOut(board.ACCELEROMETER_INTERRUPT)
lis3dh = adafruit_lis3dh.LIS3DH_I2C(i2c, address=0x19, int1=_int1)
lis3dh.range = adafruit_lis3dh.RANGE_8_G

# --- PWM Helper ---
def ms_to_duty_cycle(ms):
    # Clamps pulse between 1.0ms and 2.0ms for ESC safety
    ms = max(1.0, min(ms, 2.0))
    return int((ms / 20.0) * 65535)

# --- Setup ESC Pins ---
# Frequency must be 50Hz for standard ESCs
pwm_l = pwmio.PWMOut(board.A1, frequency=50)
pwm_r = pwmio.PWMOut(board.A2, frequency=50)

# --- Setup Arming Button ---
button = digitalio.DigitalInOut(board.BUTTON_A)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.DOWN

motor_off_ms = 1.5
armed = False

# Initialization: Send 1.1ms to initialize/arm the ESCs
print("Initializing ESCs at 1.5ms...")
init_duty = ms_to_duty_cycle(motor_off_ms)
pwm_l.duty_cycle = init_duty
pwm_r.duty_cycle = init_duty
time.sleep(2.0)

while True:
   
    x,y,z = lis3dh.acceleration
   
    pitch = math.atan2(x,z)*180/3.14159
    roll = math.atan2(y,z)*180/3.14159
   
    # 1. Check for Arming
    if button.value:
        armed = not armed
        if armed:
            print("--- SYSTEM ARMED ---")
        else:
            print("--- SYSTEM DISARMED---")
        time.sleep(2.0)

    # 4. Output Logic
    if armed:
        l_ms = motor_off_ms + pitch/900. ##1.5 is center, 1.1 is full backwards, 1.9 is full forwards
        r_ms = motor_off_ms + roll/900.  ##1.5 is center, 1.1 is full rotate C/CCW, 1.9 is full rotation CCW/CW
    else:
        # Stay at safety initialization pulse
        l_ms = motor_off_ms
        r_ms = motor_off_ms
       
    #Add a saturation block
    if l_ms > 1.9: l_ms = 1.9
    if r_ms > 1.9: r_ms = 1.9
    if l_ms < 1.1: l_ms = 1.1
    if r_ms < 1.1: r_ms = 1.1
       
    #Send command
    pwm_l.duty_cycle = ms_to_duty_cycle(l_ms)
    pwm_r.duty_cycle = ms_to_duty_cycle(r_ms)

    print(time.monotonic(),l_ms,r_ms,pitch,roll)
   
    time.sleep(0.01)
