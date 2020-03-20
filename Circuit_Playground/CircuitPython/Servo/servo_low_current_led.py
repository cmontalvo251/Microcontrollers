import time
import board
import digitalio
import pulseio
import analogio
import time

#Read analog signal
analogin = analogio.AnalogIn(board.A6)

# Initialize PWM output for the servo (on pin A2):
servo = pulseio.PWMOut(board.A3, frequency=50)

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

# Main loop will run forever moving between 1.0 and 2.0 mS long pulses:
duty = 1.0
nowtime = time.monotonic()
CHANGE = 0
while True:
    if time.monotonic() > nowtime + 1:
        nowtime = time.monotonic()
        if CHANGE == 1:
            servo.duty_cycle = servo_duty_cycle(0.4)
        if CHANGE == 0:
            servo.duty_cycle = servo_duty_cycle(0.8)
        CHANGE = not CHANGE
    #print((analogin.value,))
    time.sleep(0.5)