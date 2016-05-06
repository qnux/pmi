#include "Rod.h"
#include <wiring_private.h>
#include "constants.h"
#include "pinout.h"

Rod::Rod(Servo * arm, Servo * release) {
	m_srvArm = arm;
	m_srvRel = release;
	m_askedState = ROD_UNDEFINED;
	m_previousState = ROD_UNDEFINED;
	m_color = GRN;
	m_timeChange = 0;
	m_last_position = 0;
}

void
Rod::run()
{
	if (m_askedState != m_previousState)
	{
		m_last_position = m_srvArm->read();
		m_timeChange = millis();
		m_previousState = m_askedState;
	}

	switch (m_askedState){
	case ROD_UNDEFINED:
		m_srvRel->write(ROD_RELEASE_REST);
		m_askedState = ROD_REST;
		break;
	case ROD_REST:
		m_srvRel->write(ROD_RELEASE_REST);
		if (m_color == GRN)
			sweep(ROD_ARD_REST_GREEN);
		else
			sweep(ROD_ARD_REST_PURPLE);
		break;
	case ROD_FISH:
		m_srvRel->write(ROD_RELEASE_REST);
		if (m_color == GRN)
			sweep(ROD_ARD_FISHING_GREEN);
		else
			sweep(ROD_ARD_FISHING_PURPLE);
		break;
	case ROD_TRAVEL_WITH_FISH:
		m_srvRel->write(ROD_RELEASE_REST);
		if (m_color == GRN)
			sweep(ROD_ARD_TRAVEL_GREEN);
		else
			sweep(ROD_ARD_TRAVEL_PURPLE);
		break;
	case ROD_POSITION_DROP:
		m_srvRel->write(ROD_RELEASE_REST);
		if (m_color == GRN)
			sweep(ROD_ARD_DROP_GREEN);
		else
			sweep(ROD_ARD_DROP_PURPLE);
		break;
	case ROD_DROP:
		m_srvRel->write(ROD_RELEASE_DROP);
		if (m_color == GRN)
			sweep(ROD_ARD_DROP_GREEN);
		else
			sweep(ROD_ARD_DROP_PURPLE);
		break;
	default:;
	}
}

void
Rod::sweep(int position)
{
	float ratio = (millis() - m_timeChange) / FISH_DURATION_MS;

	if (ratio < 1.0)
	{
		m_srvArm->write((position - m_last_position) * ratio + m_last_position);
	}
	else
	{
		m_srvArm->write(position);
	}
}
