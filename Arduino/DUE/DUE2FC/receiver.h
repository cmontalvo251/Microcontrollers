/*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// This is a C-style library for the transmitter and receiver.
// It's not C++-style class because the interrupt handlers needs to be a static functions and it get's ugly if you want to do this with classes.
// The namespace is good enough.

#ifndef QC_RECEIVER_H
#define QC_RECEIVER_H

#include <Arduino.h>

#define RECV_N_CHANNEL 	6

#define RECV_CHAN0PIN 	2
#define RECV_CHAN1PIN 	3
#define RECV_CHAN2PIN 	4
#define RECV_CHAN3PIN 	5
#define RECV_CHAN4PIN 	6
#define RECV_CHAN5PIN 	7

#define RECV_PULSE_LENGTH_MIN	950
#define RECV_PULSE_LENGTH_MAX	2050

// Global interface for channel values.
// Receiver return pulse lengths for each channel within these two values.
// Should not be changed.
#define RECV_CLIP_MINIMUM 	1000.0f
#define RECV_CLIP_MAXIMUM 	2000.0f

#include "sd.h"
#include "i2c.h"

class receiver {
  public:
    int receiver_input_channel_throttle, receiver_input_channel_yaw, receiver_input_channel_pitch, receiver_input_channel_roll, receiver_input_channel_gear0,receiver_input_channel_gear1;
    int receiver_input_channel_gear0_new,receiver_input_channel_gear1_new;
    bool CHECK_MODE;
    bool QUADARMED;
    float lastReceiverTime;
    int MODE=0;
    void setup_RX();
    void poll_receiver(i2c);
    void print_SD_receiver(File);
    void print_RX();
    int get_FASTPilot_mode();

    //Constructor Class
    receiver(); 
};


enum recv_channel
{
  THROTTLE,
  AILERON,
  ELEVATOR,
  RUDDER,
  AUX0,
  AUX1
};
	
namespace receiverModule
{
  //Can I put variables into this namespace? No you cannot.
  //int MODE_RX;
  //What about static ints and then calling the receiverModule namespace?
  //this still doesn't work. I'm going to have to make classes. Dammit.
  //static volatile int MODE;
  
	void init();
	
	// you might attach an interrupt at a high pulse which can corrupt the statistics
	void resetLimitStats();
	
	// return true if meaningful pulse lengths are received on all channels
	bool allChannelsInLimits();
	
	// access pulse lengths
	float getClippedChannelValue_f(enum recv_channel index);
	
	// analyse pulse lengths
	int checkZoneClippedValue(float channelValue, int n_zones);
	bool checkLimitClippedValue(float channelValue, float center, float limits);
	
	#ifdef DBG_RECEIVER_PRINT
	void printDebug();
	#endif
}

#endif
