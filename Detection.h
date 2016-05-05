// Classe de gestion de la led multicolor
#ifndef DETECTION_H
#define DETECTION_H

#include <stdint.h>
#include "Arduino.h"

class Detection
{

public:
	Detection(int AVG, int AVD, int ARG, int ARD);

	void run();

	bool getDetectionFront(){ return m_obstacleFront; }
	bool getDetectionRear(){ return m_obstacleRear; }

	void setActiveFront(bool b) { m_activeFront = b;}
	void setActiveRear(bool b) { m_activeRear = b;}
	void disableAll() { m_activeFront = false; m_activeRear = false; }
	void disableFront() { m_activeFront = false; }
	void disableRear() { m_activeRear = false; }
	void disableLeft() { m_activeLeft = false; }
	void disableRight() { m_activeRight = false; }
	void activateAll() { m_activeFront = true; m_activeRear = true; }

private:

	bool m_obstacleFront;
	bool m_obstacleRear;

	int m_AVG;
	int m_AVD;
	int m_ARG;
	int m_ARD;

	bool m_activeFront;
	bool m_activeRear;
	bool m_activeLeft;
	bool m_activeRight;

	unsigned long m_dateDetectionFront;
	unsigned long m_dateDetectionRear;
};


#endif
// DETECTION_H
