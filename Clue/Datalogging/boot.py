import board
import digitalio
import storage
import analogio
import time

ctouch = analogio.AnalogIn(board.D0)
S = True
for i in range(0,25):
    print(ctouch.value,i)
    time.sleep(0.01)
    if ctouch.value > 2000:
        print('D0 Touch Detected')
        S = False

storage.remount("/", S)
if (S):
    print('Hold down the D0 capacative touch next time')
else:
    print('Storage changed')
