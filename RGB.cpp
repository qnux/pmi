#include "RGB.h"
#include <wiring_private.h>
#include "constants.h"

RGB::RGB(int pin_r, int pin_g, int pin_b) {
	pinMode(pin_r, OUTPUT);
	pinMode(pin_g, OUTPUT);
	pinMode(pin_b, OUTPUT);

	m_r = pin_r;
	m_g = pin_g;
	m_b = pin_b;

	m_color = WHITE;
	m_blink = ON;
	m_timeChanged = millis();
}

void
RGB::set(rgb_color_t color, rgb_blink_t blink) {
	m_color = color;
	m_blink = blink;
}

void
RGB::off() {
	m_blink = OFF;
}

void
RGB::run(){

	bool update = false;
	static int lastState = 0;
	static bool on = false;

	if (millis() - m_timeChanged >= OSCILATOR)
	{
		m_timeChanged = millis();
		lastState++;
		update = true;
	}

	if (update)
	{
		if ((lastState%2 == 0 && m_blink == FAST)
				|| (lastState%10 == 0 && m_blink == SLOW))
		{
			on = !on;
		}
		else if (m_blink == ON)
		{
			on = true;
		}
		else if (m_blink == OFF)
		{
			on = false;
		}

		output(on);
	}
}

void
RGB::output(bool on){
	if (on)
	{
		switch(m_color)
		{
		case RED:
			digitalWrite(m_r , LOW);
			digitalWrite(m_g , HIGH);
			digitalWrite(m_b , HIGH);
			break;
		case GREEN:
			digitalWrite(m_r , HIGH);
			digitalWrite(m_g , LOW);
			digitalWrite(m_b , HIGH);
			break;
		case BLUE:
			digitalWrite(m_r , HIGH);
			digitalWrite(m_g , HIGH);
			digitalWrite(m_b , LOW);
			break;
		case YELLOW:
			digitalWrite(m_r , LOW);
			digitalWrite(m_g , LOW);
			digitalWrite(m_b , HIGH);
			break;
		case CYAN:
			digitalWrite(m_r , HIGH);
			digitalWrite(m_g , LOW);
			digitalWrite(m_b , LOW);
			break;
		case PURPLE:
			digitalWrite(m_r , LOW);
			digitalWrite(m_g , HIGH);
			digitalWrite(m_b , LOW);
			break;
		case WHITE:
			digitalWrite(m_r , LOW);
			digitalWrite(m_g , LOW);
			digitalWrite(m_b , LOW);
			break;
		default:;
		}
	}
	else
	{
		digitalWrite(m_r , HIGH);
		digitalWrite(m_g , HIGH);
		digitalWrite(m_b , HIGH);
	}
}

