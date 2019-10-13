import board
import digitalio
import time
import touchio
import pulseio
from adafruit_motor import servo

# Initialize PWM output for the servo (on pin D4):
pwm = pulseio.PWMOut(board.D4,duty_cycle=2**15,frequency=50)

# Create a servo object, my_servo.  
my_servo = servo.Servo(pwm)

##Big LEDs
big_led = digitalio.DigitalInOut(board.D3)
big_led.direction = digitalio.Direction.OUTPUT
big_led.value = False

#Set up Touch Buttons
touch1 = touchio.TouchIn(board.TOUCH1)
touch2 = touchio.TouchIn(board.TOUCH2)
touch3 = touchio.TouchIn(board.TOUCH3)
touch4 = touchio.TouchIn(board.TOUCH4)

while True:
    if touch1.value or touch2.value or touch3.value or touch4.value:
        print("Touched")
        my_servo.angle = 0
        time.sleep(2)
        my_servo.angle = 180
    time.sleep(0.05)