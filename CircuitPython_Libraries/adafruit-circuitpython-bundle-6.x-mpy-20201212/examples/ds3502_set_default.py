import board
import adafruit_ds3502

i2c = board.I2C()
ds3502 = adafruit_ds3502.DS3502(i2c)


#   Steps to test `set_default`:

#   1. Run the program once to see what the current default value is
#   2. Uncomment the call to `setWiperDefault` below and run it again.
#   3. Once the program has run with the `set_default` call,
#       comment out the line below again, then powercycle the DS3502
#       by removing then reattaching VCC
#   4. Then run the program again with the line commented out to see the
#       new default wiper value

# ds3502.set_default(99)


print("Default wiper value: %d" % ds3502.wiper)