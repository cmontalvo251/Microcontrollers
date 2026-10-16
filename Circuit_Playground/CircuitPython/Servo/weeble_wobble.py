import time
import board
import busio
import digitalio
import pwmio
import math
import adafruit_lsm9ds1

# --- Setup I2C and Sensor ---
i2c = busio.I2C(board.SCL, board.SDA)
sensor = adafruit_lsm9ds1.LSM9DS1_I2C(i2c)

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

# --- PID Constants ---
kp = 0.00
ki = 0.00
kd = 0.00
motor_off_ms = 1.1
nominal_ms = 1.1
target_angle = 0.0

# Variables
integral = 0.0
last_error = 0.0
last_time = time.monotonic()
armed = False

# Initialization: Send 1.1ms to initialize/arm the ESCs
print("Initializing ESCs at 1.1ms...")
init_duty = ms_to_duty_cycle(1.1)
pwm_l.duty_cycle = init_duty
pwm_r.duty_cycle = init_duty
time.sleep(2.0)
#init filter
roll_f = 0.0
while True:
    # 1. Check for Arming
    if button.value:
        armed = not armed
        if armed:
            print("--- SYSTEM ARMED ---")
        else:
            print("--- SYSTEM DISARMED---")
        time.sleep(2.0)

    # 2. Sensor Logic
    accel_x, accel_y, accel_z = sensor.acceleration
    roll = math.atan2(accel_x, accel_z) * 180 / math.pi

    #Filter signal
    s = 0.1
    roll_f = roll*s + roll_f*(1-s)

    # 3. PID Math
    now = time.monotonic()
    dt = now - last_time
    if dt <= 0: dt = 0.001 # Prevent division by zero
    
    error = target_angle - roll_f
    
    p_term = kp * error
    
    if ki > 0:
        integral += error * dt
    i_term = ki * integral
    
    derivative = (error - last_error) / dt
    d_term = kd * derivative
    
    control = p_term + i_term + d_term

    # 4. Output Logic
    if armed:
        l_ms = nominal_ms + control
        r_ms = nominal_ms - control
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

    # 5. Serial Monitor
    print(f"Roll: {roll_f:7.3f} | P:{kp:.2f} I:{ki:.2f} D:{kd:.3f} | Left:{l_ms:.1f} Right:{r_ms:.1f} A:{armed}")

    # State Update
    last_error = error
    last_time = now
    time.sleep(0.01)
