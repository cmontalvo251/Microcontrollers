# Write your code here :-)
import time
import board
import analogio
import pwmio
import random
import neopixel
pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=0.5)

#servo stuff
servo = pwmio.PWMOut(board.A3, frequency=50)
def servo_duty_cycle(pulse_ms, frequency=50):
    period_ms = 1.0 / frequency * 1000.0
    duty_cycle = int(pulse_ms / (period_ms / 65535.0))
    return duty_cycle

##SLOTS
o = ["cherry","potato","radish"]


###TWO DIFFERENT ANALOG SIGNALS
analog = analogio.AnalogIn(board.A1)
analog2 = analogio.AnalogIn(board.A2)

##STATE MACHINE
STATE = -1
#-1 = print start game
#0 = waiting for a coin
#1 = received a coin, waiting for lever pull
#2 = coin inserted, lever pulled, spin slots
#3 = all slots are the same

while True:
    val1 = analog.value
    val2 = analog2.value
    #Sprint(val1,val2)
    if STATE == -1:
        print('Insert Coin.....')
        STATE = 0
    if STATE == 0:
        pixels.fill((255,255,255))
        val = analog.value
        #print(val)
        if val > 60000:
            print('Coin has been inserted. Waiting for lever pull...')
            STATE = 1
    if STATE == 1:
        pixels.fill((255,0,0))
        val = analog2.value
        #print(val)
        if val > 40000:
            print('Coin inserted. Level pulled. Lets run the slots....')
            STATE = 2
    if STATE == 2:
        pixels.fill((0,255,0))
        a1=random.randrange(0,3)
        a2=random.randrange(0,3)
        a3=random.randrange(0,3)
        print(o[a1],o[a2],o[a3])
        if a1==a2==a3:
            STATE = 3
        else:
            print('YOU LOOSTTT! SUCKS TO SUCK')
            for x in range(0,4):
                pixels.fill((255,0,0))
                time.sleep(0.2)
                pixels.fill((0,0,0))
                time.sleep(0.2)
            STATE = -1
    if STATE == 3:
        servo.duty_cycle = servo_duty_cycle(0.65)
        print('YOU WINNNNNNNNN QUIT YOUR DAY JOB!!!!')
        for x in range(0,4):
            pixels.fill((0,0,255))
            time.sleep(0.2)
            pixels.fill((0,0,0))
            time.sleep(0.2)
        STATE = -1
    else:
        servo.duty_cycle = servo_duty_cycle(2.3)
    time.sleep(0.1)
