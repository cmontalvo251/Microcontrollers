# The MIT License (MIT)
#
# Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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
"""
`adafruit_irremote`
====================================================

Demo code for upcoming CircuitPlayground Express:

.. code-block: python

    import pulseio
    import board
    import adafruit_irremote

    with pulseio.PulseIn(board.REMOTEIN, maxlen=120, idle_state=True) as p:
        d = adafruit_irremote.GenericDecode()
        code = bytearray(4)
        while True:
            d.decode(p, code)
            print(code)

* Author(s): Scott Shawcroft
"""

# Pretend self matter because we may add object level config later.
# pylint: disable=no-self-use

import array

class IRDecodeException(Exception):
    """Generic decode exception"""
    pass

class IRNECRepeatException(Exception):
    """Exception when a NEC repeat is decoded"""
    pass


class GenericDecode:
    """Generic decoding of infrared signals"""
    def bin_data(self, pulses):
        """Compute bins of pulse lengths where pulses are +-25% of the average.

           :param list pulses: Input pulse lengths
           """
        bins = [[pulses[0], 0]]

        for _, pulse in enumerate(pulses):
            matchedbin = False
            #print(pulse, end=": ")
            for b, pulse_bin in enumerate(bins):
                if pulse_bin[0]*0.75 <= pulse <= pulse_bin[0]*1.25:
                    #print("matches bin")
                    bins[b][0] = (pulse_bin[0] + pulse) // 2  # avg em
                    bins[b][1] += 1                 # track it
                    matchedbin = True
                    break
            if not matchedbin:
                bins.append([pulse, 1])
            #print(bins)
        return bins

    def decode_bits(self, pulses, debug=False):
        """Decode the pulses into bits."""
        # pylint: disable=too-many-branches,too-many-statements
        if debug:
            print("length: ", len(pulses))

        # special exception for NEC repeat code!
        if ((len(pulses) == 3) and (8000 <= pulses[0] <= 10000) and
                (2000 <= pulses[1] <= 3000) and (450 <= pulses[2] <= 700)):
            raise IRNECRepeatException()

        if len(pulses) < 10:
            raise IRDecodeException("10 pulses minimum")

        # remove any header
        del pulses[0]
        if len(pulses) % 2 == 1:
            del pulses[0]
        if debug:
            print("new length: ", len(pulses))

        evens = pulses[0::2]
        odds = pulses[1::2]
        # bin both halves
        even_bins = self.bin_data(evens)
        odd_bins = self.bin_data(odds)
        if debug:
            print("evenbins: ", even_bins, "oddbins:", odd_bins)

        outliers = [b[0] for b in (even_bins + odd_bins) if b[1] == 1]
        even_bins = [b for b in even_bins if b[1] > 1]
        odd_bins = [b for b in odd_bins if b[1] > 1]
        if debug:
            print("evenbins: ", even_bins, "oddbins:", odd_bins, "outliers:", outliers)

        if not even_bins or not odd_bins:
            raise IRDecodeException("Not enough data")

        if len(even_bins) == 1:
            pulses = odds
            pulse_bins = odd_bins
        elif len(odd_bins) == 1:
            pulses = evens
            pulse_bins = even_bins
        else:
            raise IRDecodeException("Both even/odd pulses differ")

        if debug:
            print("Pulses:", pulses, "& Bins:", pulse_bins)
        if len(pulse_bins) == 1:
            raise IRDecodeException("Pulses do not differ")
        elif len(pulse_bins) > 2:
            raise IRDecodeException("Only mark & space handled")

        mark = min(pulse_bins[0][0], pulse_bins[1][0])
        space = max(pulse_bins[0][0], pulse_bins[1][0])
        if debug:
            print("Space:", space, "Mark:", mark)

        if outliers:
            # skip outliers
            pulses = [p for p in pulses if not
                      (outliers[0]*0.75) <= p <= (outliers[0]*1.25)]
        # convert marks/spaces to 0 and 1
        for i, pulse_length in enumerate(pulses):
            if (space*0.75) <= pulse_length <= (space*1.25):
                pulses[i] = False
            elif (mark*0.75) <= pulse_length <= (mark*1.25):
                pulses[i] = True
            else:
                raise IRDecodeException("Pulses outside mark/space")
        if debug:
            print(len(pulses), pulses)

        # convert bits to bytes!
        output = [0] * ((len(pulses)+7)//8)
        for i, pulse_length in enumerate(pulses):
            output[i // 8] = output[i // 8] << 1
            if pulse_length:
                output[i // 8] |= 1
        return output

    def read_pulses(self, input_pulses, max_pulse=10000):
        """Read out a burst of pulses until a pulse is longer than ``max_pulse``.

           :param ~pulseio.PulseIn input_pulses: Object to read pulses from
           :param int max_pulse: Pulse duration to end a burst
           """
        received = []
        while True:
            while len(input_pulses) < 8:   # not too big (slower) or too small (underruns)!
                pass
            while input_pulses:
                pulse = input_pulses.popleft()
                if pulse > max_pulse:
                    if not received:
                        continue
                    else:
                        return received
                received.append(pulse)
        return received

class GenericTransmit:
    """Generic infrared transmit class that handles encoding."""
    def __init__(self, header, one, zero, trail):
        self.header = header
        self.one = one
        self.zero = zero
        self.trail = trail

    def transmit(self, pulseout, data):
        """Transmit the ``data`` using the ``pulseout``.

           :param pulseio.PulseOut pulseout: PulseOut to transmit on
           :param bytearray data: Data to transmit
           """
        durations = array.array('H', [0] * (2 + len(data) * 8 * 2 + 1))
        durations[0] = self.header[0]
        durations[1] = self.header[1]
        durations[-1] = self.trail
        out = 2
        for byte_index, _ in enumerate(data):
            for i in range(7, -1, -1):
                if (data[byte_index] & 1 << i) > 0:
                    durations[out] = self.one[0]
                    durations[out + 1] = self.one[1]
                else:
                    durations[out] = self.zero[0]
                    durations[out + 1] = self.zero[1]
                out += 2

        print(durations)
        pulseout.send(durations)
