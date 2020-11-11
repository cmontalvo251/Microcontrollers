import pulseio
import board
import time
import adafruit_irremote
import neopixel

pixels = neopixel.NeoPixel(board.NEOPIXEL, 10, brightness=1.0)

# Create a 'pulseio' input, to listen to infrared signals on the IR receiver
pulsein = pulseio.PulseIn(board.IR_RX, maxlen=120, idle_state=True)
# Create a decoder that will take pulses and turn them into numbers
decoder = adafruit_irremote.GenericDecode()

while True:
    pulses = decoder.read_pulses(pulsein)
    try:
        # Attempt to convert received pulses into numbers
        received_code = decoder.decode_bits(pulses)
    except adafruit_irremote.IRNECRepeatException:
        # We got an unusual short code, probably a 'repeat' signal
        print("NEC repeat!")
        continue
    except adafruit_irremote.IRDecodeException as e:
        # Something got distorted or maybe its not an NEC-type remote?
        print("Failed to decode: ", e.args)
        continue

    #print(pulses)
    #print("Infrared code received: ", received_code)
    if received_code[2] == 247:
        print("Channel Down")
        pixels.fill((255,0,0))
    elif received_code[2] == 183:
        print("Channel Up")
        pixels.fill((0,255,0))
    pixels.show()