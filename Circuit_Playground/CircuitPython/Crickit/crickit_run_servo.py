import time
from adafruit_crickit import crickit

print("1 Servo demo!")

##If you ever need to change the range of the servos you can use this
#crickit.servo_1.set_pulse_width_range(min_pulse=500, max_pulse=2500)
#default is 750 to 2250
 
x = 1 

while True:
    print(x)
    x+=1
    print("Moving servo to 0")
    crickit.servo_1.angle = 0      # right
    crickit.servo_2.angle = 0
    time.sleep(1)
    print("Moving servo to 90")
    crickit.servo_1.angle = 90     # middle
    crickit.servo_2.angle = 90
    time.sleep(1)
    print("Moving servo to 180")
    crickit.servo_1.angle = 180    # left
    crickit.servo_2.angle = 180
    time.sleep(1)
    print("Moving servo to 90")
    crickit.servo_1.angle = 90     # middle
    crickit.servo_2.angle = 90
    time.sleep(1)
    # and repeat!