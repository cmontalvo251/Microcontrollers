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

///Carlos Montalvo Downloaded this code on 12/5/2017
///He heavily commented the code and you can see his comments 
///by the fact that he uses three slashes like so ///


////////////THE QUADCOPTER LIBRARY IS IN ARDUINO/LIBRARIES/QUADCOPTER_LIB AND ALSO COPIED HERE AS WELL///////////

*/

#include <scheduler.h> ///this just has class called scheduler
#include <flightController.h> /////This includes the receiver.h

scheduler _scheduler; ///This is a class in Quadcopter_lib - there is no init routine
flightController _flightController; ///Same. No init routine

void setup()
{
  _scheduler.schedulerSetup(); ///this just sets a bunch of variables in the scheduler class
  
  ///Turns everything on and also sends 1100 to motors
  _flightController.initTimeCritical(); ///these next routines are all in flightcontroller.cpp
  ///This start up sound is beep beep beeeeep. THe buzzer is literally a dumb buzzer that just makes noise.
  //There is no "melody" it's just an annoying beep
  _flightController.playStartUpSound(); ///head over there to see what they do
  ///Ok this is probably the important part. How to read the receiver on a DUE
  ///Receiver is plugged into pins 44-49 and it is using pretty fancy interrupts to get the signals
  ///Look in the receiver.cpp function to see it.
  _flightController.startReceiverProcess(); ///Again go to receiver.cpp to see
  //_flightController.checkESCCalibration();
  ///Kick off the rategyro, accelerometer, barometer and magnetometer
  _flightController.initSensors();
  ///Make sure all sticks are down. Play some songs and calibrate sensors using the readSensors function which is also in
  ///flight controller
  _flightController.checkSensorTiltCalibration();
  ///Do a final check to make sure receiver signals are in the right position
  _flightController.checkReceiverDefaultPosition();
  
  _flightController.estimateInitialAttitude();
  _flightController.playStartUpFinishedSound();

  _scheduler.setTime();
}

void loop()
{
  // 200Hz
  //the scheduler class just checks when things are happening
  //This loop here stops once the 200 Hz limit has passed
  while(!_scheduler.checkBasePeriod());

  //As fast as possible the code reads the raw sensor signal
  _flightController.readSensors();
  //Then it fuses the sensors together
  _flightController.callSensorFusion();

  //This routine should compute a pwm_output signal
  //It does. This routine calls the compute_pwmDutyCycle() which uses the stabilizer class
  //and returns a pwm_output
  //So this routine has a PID loop with two classes one is the stabilizer class
  //the other is a PID class
  //This contoller has an inner loop outer loop controller
  //for pitch and roll
  //OUTER LOOP - Kp = 4.5
  //INNER LOOP - Kp = 0.9, Ki = 1.0
  //ANTIWINDUP - max = 50
  ///roll_stab_output = 4.5*(roll_command-roll) //this is limited by -250 to 250
  //^^This up here is actually the derivative of Euler angles
  //You then convert to body rates using the inverse of the H matrix (verified in dynamics text)
  //then you run
  //roll_out = 0.9*(roll_stab_out - roll_rate) + 1.0*integral(roll_stab_out - roll_rate)

  //There is integral windup on this set to 50.0
  //It's the same for pitch
  //Yaw is different
  //yaw_rate_command = 1.5*(yaw-yaw_command)
  //yaw_out = 2.7*(yaw_rate - yaw_rate_command)
  //this one has no integral gain so no integral wind up is needed
  _flightController.callStabilizer();

  //because this routine uses the pwm_output variable to
  //send to the motors
  //So the pwm_output is a pwm signal that ranges from 1000 to 2000
  //the callMotors routines makes sure that the signal is between 1000 and 2000
  //then it converts the signal to a 12 bit integer by multiplying by 4095/2500
  //then it constrains the values between 1000*(4095/2500) and 2500*(4095/2500)
  //then it sends that signal to the motors using analog Write
  _flightController.callMotors();

  // 100Hz
  if(_scheduler.checkDoublePeriod())
  {
  }

  // 66.66Hz
  if(_scheduler.checkTriplePeriod())
  {
  }

  // 50Hz
  if(_scheduler.checkQuadruplePeriod())
  {
    _flightController.readReceiver();
    _flightController.interpretReceiver();
  }

  // 20Hz
  if(_scheduler.checkDecuplePeriod())
  {
    _flightController.updateBuzzer();
    _flightController.readBatteryVoltage();
    _flightController.determineLoopControlSound();
  }  

  _scheduler.netExecTimeEnd();  
}
