import board
import neopixel
#from rocket import Rocket  ##THIS IMPORTS A MODULE CALLED ROCKET
import time
import busio
import digitalio
import adafruit_lis3dh
from adafruit_lsm6ds.lsm6ds33 import LSM6DS33 ###THIS IS AN ACCELEROMETER MODUEL
import adafruit_bmp280 #pressure sensor
import analogio

#1.) Plug in Clue and watch ctouch count to 25 (don't hold down D0)
#2.) Program will run in not logging mode but you will see data
#3.) Unplug
#4.) Plug in clue while holding D0 (you will see "D0 detected")
#5.) Keep holding down D0 while code.py initializes (you will see "storage changed" "file open")
#6.) Let go of D0
#7.) Put into rocket
#8.) Launch
#9.) Retrieve
#10.) Press B button (You will see "FILE CLOSED")
#11.) Remove power
#12.) Plug into computer (DO NOT TOUCH D0)
#13.) Copy *.txt file

ctouch = analogio.AnalogIn(board.D0)

###CREATE SOME SENSORS
i2c = busio.I2C(board.SCL, board.SDA)
sensor = LSM6DS33(i2c,address=0x6a) 
sensor2 = adafruit_bmp280.Adafruit_BMP280_I2C(i2c)

###SETUP LED
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##Button Presses
buttonB = digitalio.DigitalInOut(board.BUTTON_B)
buttonB.direction = digitalio.Direction.INPUT
buttonB.pull = digitalio.Pull.UP

###CHECK FOR OPENING FILE
if ctouch.value > 2000:
    file = open('Launch_Data_File.txt','w')
    FILEOPEN = True
else:
    print('Not opening file for writing')
    FILEOPEN = False


#rocket = Rocket()
while True:
    ##GET ALL DATA
    t = time.monotonic()
    x,y,z = sensor.acceleration
    T = sensor.temperature
    P = sensor2.pressure
    C = ctouch.value
    print(t,x,y,z,T,P,C)

    ###CHECK TO SEE IF A BUTTON IS PRESSED
    if buttonB.value == False:
        print('Button Pressed')
        if FILEOPEN == True:
            print('FILE CLOSED!!! Hit reset then copy data')
            file.close()
            FILEOPEN = False

    ##CHECK AND SEE IF FILE IS OPEN
    if FILEOPEN == True:
        print('Logging Data')
        #PRINT TO A FILE
        #print((t,x,y,z,p,rH,Te,T))
        output = str(t) + " " + str(x) + " " + str(y) + " " + str(z) + " " + str(P) + " " + str(T) + str('\n')
        file.write(output)
        file.flush()
    else:
        print('File is not open. Hold down the D0 Capacative touch. Then hit reset to start logging')
    
    time.sleep(0.1)
    
    #rocket.wait_for_connection()
    #while rocket.is_connected():
    #    rocket.check_for_packets()
    ############################edit here#############################################
    #time.sleep(0.1)
    ##################################################################################

