#ifndef USERPARAMS_H
#define USERPARAMS_H

///ALTITUDE HOLD GAINS
#define KI_ALTITUDE 0.01
#define KP_ALTITUDE 6.5 //Originally 100 but changed to 25.0 to lower oscillations
//This is the settling time of the height filter in seconds. This should be independent of update rate
//Originally set to 8.0 seconds but changed to 6.0 to see if it responds faster.
//changed to 2 at one point but damn it was super fucking noisy so I increased it back to 6.0 on 7/11/2019
#define HEIGHTSETTLE 6.0 
#define ALTITUDE_COMMAND_PRESSURE 0.1

////RECEIVER COMMANDS
#define REC_MIN 1000
#define REC_MAX 2000
//#define NEXT_TIME 0.005 //Base period from KyrellGod is 200 Hz which is 1/200 = 0.005
//Purposefully putting an error here so I know what line of code I am working on
//Need to change this NEXT_TIME variable to a different loop timer for every single 
//Subroutine. This NEXT_TIME variable has been replaced with PERIOD variables about 10 lines below.

///PWM OUT VALUES
#define PWM_MIN 1136.0
#define PWM_MID 1526.0
#define PWM_MAX 1908.0
#define PHI_MAX 55.0
#define THR_HOVER 1648.0

///LOOP RATES
#define I2C_PERIOD 0 //This is no longer used and I2C calls happen inside the SD card print
#define RECEIVER_PERIOD 0.1 //
#define CONTROL_PERIOD 0.1 //Control is the same as receiver since control depends on receiver right now.
#define COMMAND_PERIOD 0 //Send ESC signals asap
#define FILEPRINT_PERIOD 0.5 //It's 2 times a second now to make sure the GPS works
#define SERIALPRINT_PERIOD 5.0 //seconds -- For some reason the system wasn't working before. Better to just set the serial print to 1000 seconds and always have PRINTSERIAL set to true
#define XBEE_PERIOD 0.5
#define XBEE_VARS 2

//BOOLEANS
#define PRINTSERIAL true //set to false when doing flight tests but true when doing classroom demos --- Actually just change the SERIALPRINT_PERIOD for some reason, setting this to FALSE breaks something
//#define DEBUGPRINTS
//#define GPSDEBUG
#define USEGPS true

#endif
