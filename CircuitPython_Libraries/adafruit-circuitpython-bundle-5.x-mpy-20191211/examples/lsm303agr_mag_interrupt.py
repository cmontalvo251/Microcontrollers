import time
import board
import busio
import adafruit_lsm303agr_mag

i2c = busio.I2C(board.SCL, board.SDA)
lsm_mag = adafruit_lsm303agr_mag.LSM303AGR_Mag(i2c)
lsm_mag.interrupt_threshold = 80
lsm_mag.interrupt_enabled = True

while True:
    x_hi, y_hi, z_hi, x_lo, y_lo, z_lo, int_triggered = lsm_mag.faults

    print(lsm_mag.magnetic)
    print("Xhi:%s\tYhi:%s\tZhi:%s"%(x_hi, y_hi, z_hi))
    print("Xlo:%s\tYlo:%s\tZlo:%s"%(x_lo, y_lo, z_lo))
    print("Int triggered: %s"%int_triggered)
    print()

    time.sleep(1)
