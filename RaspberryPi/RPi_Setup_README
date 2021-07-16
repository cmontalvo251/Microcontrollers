-------------How to turn off APM-------------

sudo systemclt disable arducopter

------------------------------------------------

Alright so I am attempting to use vi here and make a README file on board the raspberry pi. wish me luck.

So first thing I did was install raspbian li

ok fuck vi. using nano again.
1.) Install Raspian Lite - Collin is going to try and use Emlid Raspbian but I am going to try just using Raspbian Lite and get arducopter from scratch

2.) Expand file system - sudo raspi-config expand filesystem - Dont use Gparted!!!

3.) sudo apt-get install gfortran - just to test if internet was working

4.) Can't get gedit or emacs and I don't want to learn vi (Scroll down to 32 I actually got emacs-nox to work) 

5.) sudo apt-get install git - need this for git clone

6.) git clone https://github.com/ArduPilot/ardupilot.git

------Btdubs from 2020---------------------

if you want to get a static ip on rpi go to
https://www.raspberrypi.org/documentation/configuration/tcpip/

or manually

sudo nano /etc/dhcpcd.conf

and uncomment these

interface eth0
static ip_address=192.168.0.4/24    
static routers=192.168.0.254
static domain_name_servers=192.168.0.254 8.8.8.8

If you want to go back to DHCP and get rid of static just comment out those line again
oh and make sure to

sudo reboot now

------------------------------------------

8.) Ran the git --config stuff to setup an ssh key. When I made the ssh key I had to pull the sd card out and copy the public key into gitlab on a computer with a gui. 
Then I ran git init and all that jazz and pulled down the FASTPilot.git Now I can push and pull on here.

9.) Created a folder called NAVIO to test code

10.) Tried to run some python navio code. No dice so far
sudo apt-get install python-pip
sudo pip install navio-builder
sudo pip install spidev
sudo pip install smbus -- this threw an error

11.) sudo apt-get install python-smbus

12.) Now I'm getting an IO read error because it can't find a certain device.

13.) Tried the python codes on Collin's RPi and the code says that it can't establish a connection with the mpu and if select lsm it says that imu.initialize does
not exist. Didn't this work before? What's going on????

14.) Turns out I can push code to the master directory btw on Collin's Rpi but I have to login every single time. Not ideal but it could work in a pinch

15.) Called Shawn - Turns out internet will probably work if we reset
the interfaces file. But he recommended to use the Emlid Raspian img
because apparently Navio decided to change all the driver names and
Shawn’s code won’t work unless you use his image. So either we use
Shawn’s image which is not well commented or we use Emlid img and try
and use Arducopter examples.  

16.) Collin is going to flash my Rpi with Emlid and I guess I’ll download the latest FASTPilot and Arducopter and start running examples. 

17.) git clone https://github.com/emlid/Navio2.git

18.) before you can compile the c++ examples you will need to run a ‘sudo apt-get update’ and ‘sudo apt-get pigpio’

19.) Alright so now I’m using the fresh new Raspian version from
Emlid. I’m going to git clone SUAM and FASTPilot so I can push and
pull code. Eventually I will set up an ssh key but for now I just want
to get to where I can test the Servo and AHRS codes. First things
first was to test internet so I ran 
Sudo apt-get install gfortran
And it’s all good.

20.) Now it’s time to get SUAM and FASTPilot.
Git clone https://gitlab.com/Montalvo/SUAM.git
Since these are private repos btw you have to login with your username
and psswd but I need to add you as a developer. Oh and also note that
“ and @ are flipped on the UK keyboard and the | is actually #. I
still can’t find the | on the UK keyboard but I’ll cross that bridge
later. 

21.) Ok then it came time to clone the Emlid directories.
git clone https://github.com/emlid/Navio2.git

22.) Then I grabbed pigpio by running
sudo apt-get pigpio

23.) I then cded into C++/Examples/AHRS and ran 
Make

24.) then ran the AHRS code and it ran with no issues
./AHRS -i mpu

25.) Time to run the servo block. So my plan is based on Collin’s
prediction is that the ESCs don’t need power since they are getting
power direct from the battery. So my idea is to power the servo with
an Arduino and connect the signal line to the Rpi. In case I get hit
by a bus. That’s my plan. 

26.) Ok so I wrote a quick Arduino script to make sure that the servo
I was using actually worked right. 

27.) Alright so I fired up the Raspberry pi and then ran the servo.cpp
script but I change the PWM_OUTPUT to 3 and I added some printf
statements using /n even though I can’t find \n on the UK keyboard. Ok
well I could compile the code but unfortunately I could not get the
Servo.cpp code to run. 

28.) I went ahead and ran the code as is but none of my print
statements would work. I checked the voltage coming out of the power
supply and I didn’t get any 5V coming out so I think collin is
right. So I went ahead and powered the servo using the arduino and
then hooked up common grounds and then plugged in the signal line into
the servo and guess what? It worked. Holy crap. I still don’t
understand why the print statements won’t work. 

29.) I wonder if It’s a cout versus printf thing. So I added
<iostream> to the #include statements and ran it again. That
worked. So for some reason printf statements are piped somewhere
else. Not sure where but for now if we want to use stdout print
statements we need to use  
#include <iostream>
using namespace std;
cout << “stuff” << endl;

30.) One thing I’m concerned about is writing code on this Rpi. It’s
really hard to do without emacs. I tried installing emacs-nox with
apt-get because emacs by itself wouldn’t work. Emacs installs the GUI
version and raspian lite has no GUI. emacs-nox worked like a
champ. Thank god. We can actually try and port different codes
over. Since I haven’t made any changes to code there’s no point in
versioning anything.  

31.) So the next step is to get the receiver library to read 5
channels. Once we do that we can use emacs to start combining codes
together.  

32.) Collin figured out how to change the keyboard by going into sudo
raspi-config. The caps lock key is probably done using xmodmap
commands. Ok actually you need to edit this file 

Sudo /etc/default/keyboard

Edit the file to have this

XKBOPTIONS="terminate:ctrl_alt_bksp,ctrl:nocaps"
And then run

sudo dpkg-reconfigure keyboard-configuration

Just hit ok for everything.

33.) Oh yea on emacs to get ctrl-enter to work you need to add a .emacs file

Cd ~
Nano .emacs

And then add this to that file

(define-key global-map [(control return)] ‘other-window)

This worked on Carlos’ RPi with the UK keyboard but it wouldn’t work on Collin’s keyboard with the UK keyboard.

34.) Collin worked on getting the RCInput code to read 4 channels and
we ran into a ton of issues. First off there is the auto variable. 

auto rcin = get_rcin();

This line of code is basically saying hey run this function get_rcin
which returns a pointer. The get_rcin function has an if statement
that says it will return a Navio2 pointer if the get_navio_version is
a NAVIO2 and it will return a Navio pointer if it’s the latter. This
is just to make sure it runs on both Navio2 and Navio+ boards. Since
we have a Navio2 we can actually comment this entire if statement
out. Furthermore if you look up at the header files 

#include <Navio2/RCInput_Navio2.h>
#include <Navio+/RCInput_Navio.h>

You only need the Navio header file that you’re using. So you can
actually just comment on of those out. Now back to the auto
variable. If you go to the RCInput_Navio2.h header file you’ll notice
that there is a class in there that is RCInput_Navio2 however, the
class is RCInput_Navio2 but the public class in RCInput. This is an
inherited class. I don’t know much about it but if you look in the
public declarations they have initialize and read routines that are
overrides. The reason is because the actual header for RCInput is in
Common/RCInput.h If you navigate over there you’ll see the actual
fundamental class is here. If you want to add any variables to the
RCInput class you have to do it here. 

There’s another thing though. In the makefile for RCInput the all:
routine says to MAKE ../../Navio all which means it will make all of
the cpp files in the common libraries. There is a fundamental problem
here with makefiles in that if you edit a header file the makefile
will not catch this and it won’t compile the libraries again. To get
around this we added some rm commands to delete the modules (*.o)
files in the common libraries folders everytime we run make so that we
gaurantee it’s using everything. Actually now that I think about it we
should just add that to the clean: routine so we can run make clean
and delete all the modules. Anyway, if you change the RCInput.h header
file in the common block and recompile all the modules you should be
able to get the Navio to read multiple channels. Next step is
commanding multiple servos. 

35.) Created a FASTNavio subdirectory in FASTPilot. Need to add an SSH
key to Collin’s Rpi so we can push and pull code. Or actually I’ll
just login everytime he needs to push or pull. That will be
easier. Just added FASTPilot to Collin’s Rpi so now it is tracking
changes on his Rpi. 

36.) It’s been a while since we updated this but basically we got the
receiver to read multiple channels and we got the code to command
multiple motors. We started coding the PID controller but we ran into
issues with the Kalman filter. Turns out we had to use the Madgwick
filter rather than the standard AHRS filter that comes with the Navio
examples. I called Shawn from Kansas city to get this working and
we’re working on getting the IMU lined up. Originally we had the gains
tuned properly using the Auto_tune.m code in the Dropbox folder but we
kept getting really twitchy dynamics. Turns out the data we were
working on was way too noisy and it was causing the entire filter to
mess up. We’ve now implemented the Madgwick filter and also a first
order filter implemented using Euler’s method to get rid of noise in
the angular rate data. Again all of this code is now on Gitlab so you
really just need to look in the MainLoop to see the code. I will add
in notes here periodically but it isn’t really necesary since we are
now commenting our MainLoop code pretty significantly.  

37.) Ported this readme over to the raspberry pi now that we can push
and pull code on it.

38.) Ideas for next Tuesday

- What did KyrellGod do?
Alright so I started looking into KyrellGod - So he has a
scheduler and the motors, sensor fusion and control law all run as
fast as possible but the receiver only runs at 50 Hz so we should
change that. We can probably even do 25 Hz. He also has a sensor
fusion algorithm with a complimentary filter on RPY and RPYdot. So if
we use the Madgwick filter on RPY and then a complimentary filter on
RPYdot we should be good to go.

I have heavily commented the KyrellGod_DUE code in case you'd like to understand

- Does Travis have a working code on a working drone for Mechatronics?  Maybe call Shawn Herrington on Monday?
I don't think we need to call them. KyrellGod got it. We just need to copy what he did

- When you are testing all of this make sure to only do 1 print statement so it doesn't tie up resources
- Check angles for mad_roll and mad_pitch.
Added to Action items

- Check that gx_filtered and gy_filtered is working right and gz_filtered
- Add in that test to make sure the time_constant*elapsedTime < 0.99 otherwise set it to 0.99 - ok actually change this to the complimentary filter from KyrellGod or just get rid of it.
Complimentary filter implemented by KyrellGod is completely different than this so I feel as though we should try that way first

- What if we don't let throttle go below throttle. Like say
     if pwm_signal < throttle:
      	pwm_signal = throttle
     this way the motor won't lose lift and we won't just fall out of
     the sky.
^^^I don't think we need to do this but I do think we should think about our minimum value sent to the motors to make sure we don't let the
motors turn off. So we need to figure out what the minimum spinning speed is.
judging by the data from our guess and check experiment we shouldn't let the motors go below 1250

- What if we only add to one side? You know
       if (pid_roll > 0):
          left_motor = throttle + pid_roll
	  right_motor = throttle
     else:
	  right_motor = throttle - pid_roll
	  left_motor = throttle
^^^I also don't think we should do this because KyrellGod doesn't do this. 

- Google some forums on how people get all of this to work and see if there is something fundamental that we're missing
I don't think we will find anything but KyrellGod was what we needed to look at for sure.

40.) ACTION ITEMS for Tuesday

-add constrain function using the ? operator

-Change print statements to one print statement to no tie up resources

-Check directions on roll/pitch/yaw as well as roll/pitch/yaw_rates
-Ok so turns out that yaw totally doesn't work at all. I think we should just go back to the AHRS filter from before.

-Check that the gyro is measuring angular rates in the body frame if not we need to use the H
matrix 

-Change receiver to 25 Hz (Kyrell uses 50 Hz but I think we can get away with 25)

-Tune complimentary filter - So apparently the RPY has a complimentary
filter and it varies from 0.995 to 0.998. When the quad tiltes it
filters out more noise or rather it believes model more than the data

The way it works is like this:
a.) Obtain raw measurements from accel, gyros and magn
b.) Compute roll and pitch using accel and yaw using magn
c.) compute model for roll pitch and yaw using euler integration
roll_model = roll_model + roll_rate*dT
d.) Filter the roll_model and the roll_accel (from accelerometers)
roll_est = a*roll_model + (1-a)*roll_accel

Curious how we can implement something similar. They basically aren't
filtering the angular velocity but they are using it to filter the
roll angle but since we're using something more sophisticated like the
Madgwick filter does it really matter?

-Use inner loop outer loop controller
So the way it works is like this:
a.) Compute a phidot_command = 4.5*(roll_command - roll_est) - Constrained to +-250 deg/s
b.) Convert to body frame pqr_command = inv(H)*ptpdot_command
BodyRates.x = eulerRatesDesired.x                               -         sin_theta*eulerRatesDesired.z;
BodyRates.y =                       cos_phi*eulerRatesDesired.y + sin_phi*cos_theta*eulerRatesDesired.z;
BodyRates.z =                     - sin_phi*eulerRatesDesired.y + cos_phi*cos_theta*eulerRatesDesired.z;
c.) compute roll_out = 0.9*(p_command - roll_rate_gyro) + 1.0*integral(p_command - roll_rate_gyro) - constrained to +- 500
d.) Then send the commands to the motors
    	float pwm0 = thrust_out + roll_out - pitch_out + yaw_out;
	float pwm1 = thrust_out - roll_out - pitch_out - yaw_out;
	float pwm2 = thrust_out - roll_out + pitch_out + yaw_out;
	float pwm3 = thrust_out + roll_out + pitch_out - yaw_out;
e.) Finally these are constrained to be within minimum spinning speed 1100 and 2000 (max)
We should set ours to (1250,2000)

-To get yaw_out you need

#define wrap_Pi(x) (x < -Pi ? x+Pi2 : (x > Pi ? x - Pi2: x)) //Wow this is a nested conditional
#define wrap_180(x) (x < -180 ? x+360 : (x > 180 ? x - 360: x))

yaw_error = wrap_180(yawLock-yaw_angle)
yaw_stab_output = 1.5*yaw_error - constrain from -180 to 180

if yaw_rx > limit {
   yaw_stab_output = yaw_rx - mapped from -135 to 135
   yawLock = yaw_angle
}

yaw_rate_command = inv(H)*yaw_stab_output
yaw_out = 2.7*(yaw_rate_command - yaw_rate_gyro)

-If this still doesn't work maybe we need to add threading.

41.) Ok 2/26/2019 - doing everything from number 40

-Added constrain function using the ternary operator
-Changed print statements to not tie up resources
-Added in the ability to switch between the Madgwick filter and the AHRS filter by just commenting out a variable
-I started checking the AHRS filter and the yaw channel is still broken. I must have broken something that has nothing to
do with the Madwick filter. Gonna need to see what's going on somehow.
-Alright I finished getting the filter to work. Turns out the AHRS algorithm does not use the magnetometer so I have it commented out.
-The rate gyro is measuring in the body frame so no need to convert that
-Receiver is now reading at 10Hz and it's doing just fine
-After looking at the rate gyro data I don't think we need the filter that KyrellGod uses. I think the AHRS filter is just fine.
-Added the control routine from KyrellGod. Currently debugging it to make sure the signs are right.

time, elapsed time, roll, pitch, yaw, gx, gy, gz, thrx, rollrx, pitchrx, yawrx, arm

frontleft, frontright, backright, backleft

Alright well I had to do some sign switching some gain tuning and then add a very fast complimentary filter. In the end the quad flies ok.
