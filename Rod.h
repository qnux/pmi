// Classe de gestion de la led multicolor
#ifndef ROD_H
#define ROD_H

#include <stdint.h>
#include "Arduino.h"
#include <Servo.h>
#include "constants.h"

typedef enum {
	ROD_UNDEFINED,
	ROD_REST,
	ROD_FISH,
	ROD_TRAVEL_WITH_FISH,
	ROD_POSITION_DROP,
	ROD_DROP
} rode_state_t;

class Rod
{

public:
	Rod(Servo * arm, Servo *  release);

	void rest(){m_askedState = ROD_REST;}
	void fish(){m_askedState = ROD_FISH;}
	void travelFish(){m_askedState = ROD_TRAVEL_WITH_FISH;}
	void positionDrop(){m_askedState = ROD_POSITION_DROP;}
	void drop(){m_askedState = ROD_DROP;}
	void setColor(color_t c){ m_color = c;}

	void run();

	void sweep(int position);

private:
	rode_state_t m_askedState;
	rode_state_t m_previousState;
	color_t m_color;
	Servo * m_srvArm;
	Servo * m_srvRel;
	unsigned long m_timeChange;
	int m_last_position;
};


#endif
