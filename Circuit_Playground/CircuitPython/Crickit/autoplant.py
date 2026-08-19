import time
import board
import analogio
import digitalio
from adafruit_circuitplayground import cp

# BLE Imports
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

# Setup BLE Radio and UART Service
ble = BLERadio()
ble.name = "CPB Water Sensor"
uart = UARTService()
advertisement = ProvideServicesAdvertisement(uart)

# Analog Input Setup (Nail 2 on Pad A1)
moisture_pin = analogio.AnalogIn(board.A1)

# Pump Control Pin Setup (Pad A2)
pump = digitalio.DigitalInOut(board.A2)
pump.direction = digitalio.Direction.OUTPUT
pump.value = False  # Keep pump off initially

DRY_VALUE = 2000
WET_VALUE = 45000

target_level = 10
last_ble_send = 0

def map_range(value, in_min, in_max, out_min, out_max):
    """Maps a value from one range to another."""
    scaled = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
    return int(max(out_min, min(out_max, scaled)))

while True:
    # --- BLE Advertising ---
    if not ble.connected and not ble.advertising:
        ble.start_advertising(advertisement)

    # --- Button & Control Inputs ---
    if cp.button_a:
        if target_level > 0:
            target_level -= 1
        time.sleep(0.2)

    if cp.button_b:
        if target_level < 10:
            target_level += 1
        time.sleep(0.2)

    # Determine LED active color based on switch
    switch_state = cp.switch
    active_color = (255, 0, 0) if switch_state else (0, 0, 255)

    # --- Read Moisture Sensor ---
    raw_value = moisture_pin.value
    current_water_level = map_range(raw_value, DRY_VALUE, WET_VALUE, 0, 10)

    # --- Pump Logic ---
    # Turn pump ON if current level is below desired level.
    # Turn pump OFF once current level is equal to or greater than desired level.
    if current_water_level < target_level:
        pump.value = True
    else:
        pump.value = False

    # --- Update NeoPixel Display ---
    num_leds = min(current_water_level, target_level)
    for i in range(10):
        if i < num_leds:
            cp.pixels[i] = active_color
        else:
            cp.pixels[i] = (0, 0, 0)

    # --- BLE Data Transmission ---
    if ble.connected:
        current_time = time.monotonic()
        if current_time - last_ble_send >= 1.0:
            msg = (
                f"Desired Level: {target_level}/10 | "
                f"Current Level: {current_water_level}/10 | "
                f"Pump: {'ON' if pump.value else 'OFF'} | "
                f"Switch: {'Red' if switch_state else 'Blue'}\n"
            )
            uart.write(msg.encode("utf-8"))
            last_ble_send = current_time

    time.sleep(0.05)