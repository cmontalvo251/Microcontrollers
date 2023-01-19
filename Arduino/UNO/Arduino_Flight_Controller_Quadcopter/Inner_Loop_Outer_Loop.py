import matplotlib.pyplot as plt
import numpy as np
plt.close("all")

##Get PHIMAX
PHI_MAX = np.arctan(200./244.)*180./np.pi

print 'Phi Max = ',PHI_MAX

##PLOT PHI_CMD

##Limits on PWM
PWM_MIN = 1136.
PWM_MID = 1526.
PWM_MAX = 1908.

#Slope
m = 2*PHI_MAX/(PWM_MAX - PWM_MIN)

#Run from MIN to MAX
PWM_IN = np.linspace(PWM_MIN,PWM_MAX,100)

#Expand about midpoint
PHI_CMD = m*(PWM_IN - PWM_MID)

plt.plot(PWM_IN,PHI_CMD)
plt.ylabel('Roll Angle Command (deg)')
plt.xlabel('PWM (us)')
plt.grid()

##Plot the inverse as well
PHI_CMD = np.linspace(-PHI_MAX,PHI_MAX,100)
PWM_OUT = PHI_CMD/m + PWM_MID
plt.figure()
plt.plot(PHI_CMD,PWM_OUT)
plt.xlabel('Roll Angle Command (deg)')
plt.ylabel('PWM (us)')
plt.grid()

plt.show()