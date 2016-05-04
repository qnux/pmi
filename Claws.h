// Classe de gestion de la led multicolor
#ifndef CLAWS_H
#define CLAWS_H

#include <stdint.h>
#include "Arduino.h"
#include <Servo.h>

typedef enum {
	CLAWS_CLOSE,
	CLAWS_OPEN
} claws_state_t;

class Claws
{

	typedef enum {
		C_UNDEFINED,
		C_CLOSED,
		C_OPENING,
		C_OPENED,
		C_CLOSING
	} auto_claws_state_t;

public:
	Claws(Servo * left_claw, Servo *  right_claw);

	void open(){m_askedState = CLAWS_OPEN;}
	void close(){m_askedState = CLAWS_CLOSE;}

	void run();

private:

	Servo * m_srvLeft;
	Servo * m_srvRight;

	claws_state_t m_askedState;
	auto_claws_state_t m_currentState;
	unsigned long m_timeChange;
};


#endif
