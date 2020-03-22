import time
import board
import pulseio
import digitalio
#from analogio import AnalogOut

# Initialize PWM output for the servo (on pin A2):
servo = pulseio.PWMOut(board.A2, frequency=50)

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

led_ext = digitalio.DigitalInOut(board.A3)
led_ext.direction = digitalio.Direction.OUTPUT

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

# Main loop will run forever moving between 1.0 and 2.0 mS long pulses:
duty = 1.0
while True:
    led.value = True
    led_ext.value = True
    duty = 0.8
    print('Duty Cycle = ',duty)
    servo.duty_cycle = servo_duty_cycle(duty)
    time.sleep(1.0)
    led.value = False
    led_ext.value = False
    duty = 2.0
    print('Duty Cycle = ',duty)
    servo.duty_cycle = servo_duty_cycle(duty)
    time.sleep(1.0)