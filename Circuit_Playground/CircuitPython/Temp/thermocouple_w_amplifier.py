import time
import board
import digitalio
import adafruit_max31856

##PRINT BOARD SPECIFICS
print(dir(board))
print('MISO Pin = ',board.MISO)
print('MOSI Pin = ',board.MOSI)
print('SCK Pin = ',board.SCK)

#Print Setting up SPI
spi = board.SPI()
cs = digitalio.DigitalInOut(board.A5)
cs.direction = digitalio.Direction.OUTPUT

##SETUP THE THERMOCOUPLE
thermocouple = adafruit_max31856.MAX31856(spi,cs)

while True:
    print(time.monotonic(),thermocouple.temperature)
    time.sleep(1.0)
