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

#include "global.h"
#include "complementaryFilter.h"

complementaryFilter::complementaryFilter()
{	
	// starting point has to be set via 'setCombinedEstimation()'
	combinedEstimation = 0.0f;
	
	// the loop time is fixed during runtime
	dT = GLB_LOOP_TIME/1000000.0f;

	// this setting means: trust the linear input, distrust the derivative
	setTauViaA(1.0f);
}

complementaryFilter::~complementaryFilter()
{
}

void complementaryFilter::setCombinedEstimation(float combEstim)
{
	combinedEstimation = combEstim;
}

float complementaryFilter::getCombinedEstimation(float estimation, float estimation_derivative)
{
  //So this is a derivative filter to get the RPY
  //So basically the routine works like this
  //Get the raw accel,gyro,magnetometer
  //use accel to get Roll and Pitch
  //Get complimentary filter constants using the tilt from 0.995 to 0.998
  //Then get the raw angular velocity values and use those as the estimation derivative
  //Technically they are using Euler's Method to get the new estimate so we could change this to this

  combinedEstimation = a*(combinedEstimation + estimation_derivative*dT) + (1.0f-a)*estimation;

  float kalman_estimate = combinedEstimation + dT*estimation_derivative;

  //Let's take a look at the inputs
  //combinedEstimation = previous estimate
  //estimation = Raw Roll/Pitch/Yaw from sensors - with lot's of noise
  //Oh wait this is like some wierd kalman filter
  //so really it's

  //old_estimate + dT*derivative = kalman_estimate <- that's euler's method which is really the model integrating

  //then the routine below says believe the model rather than the new estimate (estimation) because
  //the new estimate has a ton of noise.

  //But this is only for Roll Pitch Yaw yes?
  //Yea it looks like this is just RPY. The RPYdot doesn't have a filter at all
  combinedEstimation = a*kalman_estimate + (1.0f-a)*estimation;
	
	return combinedEstimation;
}

float complementaryFilter::getTau()
{
	return tau;
}

void complementaryFilter::setTau(float tau_arg)
{
	tau = tau_arg;
	a = tau/(tau+dT);
}

void complementaryFilter::setTauViaA(float a_arg)
{
  //So this a_arg will be either 0.995 or 0.998
	a = a_arg;
	//This conditional statements says if a != 1 set tau = a*dT/(1-a)
	//otherwise tau = -1??? Why -1? I guess let's see how they use tau here
	//alright so the limits on tau will be
	//2.4950 //That's quite a range.. Hmmm.
	//0.995
	tau = (a != 1.0f) ? a*dT/(1.0f-a) : COMPLEMENTARY_FILTER_TAU_UNDEFINED;
}

float complementaryFilter::getA()
{
	return a;
}
