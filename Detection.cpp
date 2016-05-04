#include "Detection.h"
#include <wiring_private.h>
#include "constants.h"
#include "pinout.h"

Detection::Detection(int AVG, int AVD, int ARG, int ARD) {
	m_AVG = AVG;
	m_AVD = AVD;
	m_ARG = ARG;
	m_ARD = ARD;

	m_obstacleFront = false;
	m_obstacleRear  = false;
	m_activeFront = true;
	m_activeRear = true;

	m_dateDetectionFront = 0;
	m_dateDetectionRear = 0;
}

void
Detection::run()
{
#if ENABLE_DETECTION
	m_obstacleFront = (digitalRead(m_AVG) || digitalRead(m_AVD));
	m_obstacleRear  = (digitalRead(m_ARG) || digitalRead(m_ARD));
#else
	m_obstacleFront = false;
	m_obstacleRear = false;
#endif

	m_obstacleFront &= m_activeFront;
	m_obstacleRear &= m_activeRear;

	// Maintain detection after release of obstacle
	if (m_obstacleFront) m_dateDetectionFront = millis();
	if (m_obstacleRear) m_dateDetectionRear = millis();

	m_obstacleFront = (millis() - m_dateDetectionFront < DELAY_RESTART_DETECTION_MS);
	m_obstacleRear = (millis() - m_dateDetectionRear < DELAY_RESTART_DETECTION_MS);
}
