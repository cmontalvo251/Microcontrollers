import time
import board
import pwmio

# Initialize PWM output for the servo (on pin A2):
servo = pwmio.PWMOut(board.A3, frequency=50)

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

# Main loop will run forever moving between 1.0 and 2.0 mS long pulses:
while True:
    for pulse_ms in [0.8,0.8]:
        print('Milli Second Pulse = ',pulse_ms)
        servo.duty_cycle = servo_duty_cycle(pulse_ms)
        time.sleep(1.0)