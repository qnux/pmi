#include "Claws.h"
#include <wiring_private.h>
#include "constants.h"
#include "pinout.h"

Claws::Claws(Servo * left_claw, Servo *  right_claw) {
	m_srvLeft = left_claw;
	m_srvRight = right_claw;
	m_askedState = CLAWS_CLOSE;
	m_currentState = C_UNDEFINED;
}

void
Claws::run()
{
	// transition
	switch (m_currentState)
	{
	case C_UNDEFINED:
		m_currentState = C_OPENING;
		break;
	case C_OPENING:
	case C_OPENED:
		if (m_askedState == CLAWS_CLOSE)
		{
			m_currentState = C_CLOSING;
			m_timeChange = millis();
		}
		break;
	case C_CLOSED:
	case C_CLOSING:
		if (m_askedState == CLAWS_OPEN)
		{
			m_currentState = C_OPENING;
			m_timeChange = millis();
		}
		break;
	default:;
	}

	// state
	switch (m_currentState)
	{
	case C_OPENING:
		m_srvLeft->write(CLAW_LEFT_OPENED);
		if (millis() - m_timeChange > DELAY_MS_CLAWS)
		{
			m_srvRight->write(CLAW_RIGHT_OPENED);
			m_currentState = C_OPENED;
		}
		break;
	case C_CLOSING:
		m_srvRight->write(CLAW_RIGHT_CLOSED);
		if (millis() - m_timeChange > DELAY_MS_CLAWS)
		{
			m_srvLeft->write(CLAW_LEFT_CLOSED);
			m_currentState = C_CLOSED;
		}
		break;
	default:;
	}
}
