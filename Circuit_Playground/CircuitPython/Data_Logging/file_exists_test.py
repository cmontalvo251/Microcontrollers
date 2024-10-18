import time
import os

number = 0
#filename = 'output' + str(number) + '.txt'
filename = 'boot_out.txt'

print('Checking for filename = ',filename)
try:
    os.stat(filename)
    FILE_EXISTS = True
except:
    FILE_EXISTS = False

print('This file exists = ',FILE_EXISTS)
