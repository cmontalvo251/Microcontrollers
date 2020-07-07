# Copyright (c) 2017 Dan Halbert for Adafruit Industries
# Copyright (c) 2017 Kattni Rembor, Tony DiCola for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Circuit Playground Sound Meter
import array
import math
import audiobusio
import board
import time

# Number of samples to read at once.
NUM_SAMPLES = 160
num_bits = 16

def normalized_rms(values):
    meanbuf = int(mean(values))
    samples_sum = sum(float(sample - meanbuf) * (sample - meanbuf) for sample in values)
    rms_mean = math.sqrt(samples_sum/len(values)) ##Notice that samples_sum = (sample-mean)**2
    return rms_mean

def mean(values):
    return sum(values) / len(values)

mic = audiobusio.PDMIn(board.MICROPHONE_CLOCK, board.MICROPHONE_DATA,sample_rate=16000, bit_depth=num_bits)
# Record an initial sample to calibrate. Assume it's quiet when we start.
samples = array.array('H', [0] * NUM_SAMPLES) #8 bit must be in ByteArray format or B, 16 bit must be in Hexadecimal, only 8 or 16 bit sampling is supported
mic.record(samples, len(samples))
input_floor = 50

while True:
    time.sleep(0.01)
    mic.record(samples, len(samples))
    m = normalized_rms(samples)
    print((m,))