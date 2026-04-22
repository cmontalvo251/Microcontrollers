import time
import board
import digitalio

# Pin Setup
DT = digitalio.DigitalInOut(board.A1)
SCK = digitalio.DigitalInOut(board.A2)

DT.direction = digitalio.Direction.INPUT
SCK.direction = digitalio.Direction.OUTPUT
SCK.value = False

def read_hx711():
    # Wait for the HX711 to be ready (DT goes LOW)
    while DT.value:
        pass

    count = 0
    # Read 24 bits of data
    for _ in range(24):
        SCK.value = True
        count = count << 1
        SCK.value = False
        if DT.value:
            count += 1
    
    # The 25th pulse sets the gain for the next reading (Channel A, Gain 128)
    SCK.value = True
    SCK.value = False

    # HX711 outputs 24-bit 2's complement
    if count & 0x800000:
        count -= 0x1000000
        
    return count

def read_average(samples=10):
    total = 0
    for _ in range(samples):
        total += read_hx711()
    return total / samples

def tare(samples=20):
    print("Calculating offset... Remove all weight.")
    total = 0
    for _ in range(samples):
        total += read_hx711()
    return total / samples

# --- Main Program ---

# 1. Tare the scale
print("Remove all weight...")
time.sleep(3)
offset = tare()
print("Offset:", offset)

# 2. Calibration factor
# You will need to adjust this scale value using a known weight
scale = 12000 

while True:
    reading = read_average(10)
    weight = (reading - offset) / scale
    
    print("Weight:", weight, "kg")
    time.sleep(0.3)
