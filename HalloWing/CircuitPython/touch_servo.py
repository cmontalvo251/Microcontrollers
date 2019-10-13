import board
import digitalio
import time
import touchio
import pulseio

# Initialize PWM output for the servo (on pin D4):
servo = pulseio.PWMOut(board.D4, frequency=50)

# Create a function to simplify setting PWM duty cycle for the servo:
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

#Set up Touch Buttons
print(dir(board))
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

while True:
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
        servo.duty_cycle = servo_duty_cycle(0.8)
        time.sleep(2)
        servo.duty_cycle = servo_duty_cycle(2.0)
    time.sleep(0.05)