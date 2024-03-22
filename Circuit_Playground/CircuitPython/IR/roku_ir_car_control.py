import pulseio
import board
import time
import adafruit_irremote

# Create a 'pulseio' input, to listen to infrared signals on the IR receiver
pulsein = pulseio.PulseIn(board.IR_RX, maxlen=120, idle_state=True)
# Create a decoder that will take pulses and turn them into numbers
decoder = adafruit_irremote.GenericDecode()
print('Decoder Loaded')

steer = 0
velocity = 0

while True:
    print('Searching for pulses....')
    pulses = decoder.read_pulses(pulsein)
    try:
        # Attempt to convert received pulses into numbers
        received_code = decoder.decode_bits(pulses)
    except adafruit_irremote.IRNECRepeatException:
        # We got an unusual short code, probably a 'repeat' signal
        print("NEC repeat!",time.monotonic())
        continue
    except adafruit_irremote.IRDecodeException as e:
        # Something got distorted or maybe its not an NEC-type remote?
        print("Failed to decode: ", e.args,time.monotonic())
        continue

    #print(pulses)
    print("Infrared code received: ", received_code,time.monotonic())

    ##CHECK FOR SPEED UP/DOWN AND TURN LEFT / RIGHT
    if received_code[2] == 103:
        velocity += 1
    if received_code[2] == 51:
        velocity -= 1
    if received_code[2] == 135:
        steer -= 1
    if received_code[2] == 75:
        steer += 1   
    print('Steer = ',steer,' Velocity = ',velocity)
