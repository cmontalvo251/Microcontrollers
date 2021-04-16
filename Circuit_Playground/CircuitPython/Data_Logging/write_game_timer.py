import time
import board
import digitalio

buttonA = digitalio.DigitalInOut(board.D4)
buttonA.direction = digitalio.Direction.INPUT
buttonA.pull = digitalio.Pull.DOWN

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
led.value = True

##Keep track of whether or not a file is open or not
FILEOPEN = False

##Loop forever
while True:
    ###check for a button press (A button)
    ##If the button is pressed we need to do one of two things
    print(FILEOPEN)
    if buttonA.value == True:
        print('Button Pressed')
        ###if the file is not open open the file and reset the timer (append)
        if FILEOPEN == False:
            print('Game Started Good Luck')
            start_time = time.monotonic()
            print(start_time)
            file = open('Ball_Maze.txt','a')
        elif FILEOPEN == True:
            ###if file is open it means we're playing the game which means we just finished
            ##at which point we need to log the length of time to the disc and close the file
            print('Game concluded')
            game_length = time.monotonic() - start_time
            print(game_length)
            output = str(game_length) + '\n'
            file.write(output)
            file.flush()
            file.close()
        while buttonA.value == True:
            print('Let go of the button bro')
            time.sleep(1.0)
            pass
        FILEOPEN = not FILEOPEN
    time.sleep(0.1)