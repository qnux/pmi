#include "Navigation.h"
#include <wiring_private.h>
#include "constants.h"
#include "pinout.h"
#include <math.h>

Navigation::Navigation(AccelStepper* sG, AccelStepper* sD)
{
	m_x = 0.0;
	m_y = 0.0;
	m_t = 0.0;
	m_inProgress = 0;
	stepperG = sG;
	stepperD = sD;
}

/**
Go to (x,y) on the table using straight lines
t is the absolute orientation of the destination
 */
int
Navigation::go_s(float x, float y, int sens) {

	float alpha = 0.0;
	static float angle = 0.0;
	static float delta = 0.0;
	static int state = 0;
	static int prev_state = -1;

	if (prev_state != state)
	{
		Serial.print("Navigation::go_s state");
		Serial.print(prev_state);
		Serial.print(" -> ");
		Serial.println(state);
		Serial.print("odom: ");Serial.print(m_x);Serial.print(",");Serial.print(m_y);Serial.print(",");Serial.println(m_t);
		prev_state = state;
	}

	switch (state)
	{
	case 0: // init
		state = 1;
		break;
	case 1: // compute first turn
		alpha = atan2(y-m_y, x-m_x);
		alpha *= sens;
		Serial.print("alpha = "); Serial.println(alpha);
		if (alpha-m_t < M_PI)
			angle = alpha - m_t + 2* M_PI;
		else if (alpha-m_t > M_PI)
			angle = alpha - m_t - 2* M_PI;
		else
			angle = alpha-m_t;
		startTraj();
		state = 2;
		break;
	case 2:
		if (turn(angle))
		{
			state = 3;
		}
		break;
	case 3:
		delta = sens*sqrt((x-m_x)*(x-m_x)+(y-m_y)*(y-m_y));
		Serial.print("delta = "); Serial.println(delta);
		startTraj();
		state = 4;
		break;
	case 4:
		if(straight(delta))
		{
			state = 0;
		}
		break;
	default:;
	}

	if (state == 0)
	{
		Serial.print("I HAVE ARRIVED IN ");
		Serial.print(x);
		Serial.print(",");
		Serial.println(y);
	}

	return (state == 0);
}

/**
Go to (x,y) on the table using straight lines
t is the absolute orientation of the destination
 */
int
Navigation::go_s(float x, float y, float t, int sens) {

	static float angle = 0.0;
	static int state = 0;
	switch (state)
	{
	case 0: // init
		startTraj();
		state = 1;
		break;
	case 1:
		if (go_s(x,y))
			state = 2;
		break;
	case 2:
		if ( t - m_t < M_PI)
			angle =  t - m_t + 2* M_PI;
		else if ( t - m_t > M_PI)
			angle =  t - m_t - 2* M_PI;
		else
			angle =  t - m_t;
		state = 3;
		break;
	case 3:
		if(turn(angle))
			state = 0;
		break;
	default:;
	}

	return (state == 0);
}

int
Navigation::straight(float mm)
{
	stepperG->setMaxSpeed(V_MAX);
	stepperD->setMaxSpeed(V_MAX);
	stepperG->setAcceleration(ACC_MAX);
	stepperD->setAcceleration(ACC_MAX);
	stepperG->moveTo(-mm * GAIN_STEP_MM);
	stepperD->moveTo(mm * GAIN_STEP_MM);

	if (isArrived())
	{
		// Update odom
		m_x += mm * cos(m_t);
		m_y += mm * sin(m_t);
		return true;
	}
	else
		return false;
}

int
Navigation::turn(float angle)
{
	stepperG->moveTo(angle * VOIE/2 * GAIN_STEP_MM);
	stepperD->moveTo(angle * VOIE/2 * GAIN_STEP_MM);

	if (isArrived())
	{
		// Update odom
		m_t += angle;
		return true;
	}
	else
		return false;
}

void
Navigation::motorRun()
{
	stepperG->run();
	stepperD->run();
	//@todo compute_odom
}


void
Navigation::compute_odom(float dxG,float dxD)
{
	float ds = (dxG + dxD)/2;
	float dtheta = (dxD-dxG)/VOIE;

	m_x += ds * cos(m_t + dtheta/2);
	m_y += ds * sin(m_t + dtheta/2);
	m_t += dtheta;

	Serial.print("odom: G+");Serial.print(dxG);Serial.print("  D+");Serial.println(dxG);
	Serial.print("odom: ");Serial.print(m_x);Serial.print(",");Serial.print(m_y);Serial.print(",");Serial.println(m_t);
}

/**
 * return true if the trajectory is finished
 */
int
Navigation::isArrived()
{
	return (stepperG->distanceToGo() == 0 || stepperD->distanceToGo() == 0);
	return 0;
}

/**
 * RAZ position of steppers in order to begin new trajectory
 * this also update variables for the odometry
 */
void
Navigation::startTraj()
{
	stepperG->setCurrentPosition(0);
	stepperD->setCurrentPosition(0);
}
