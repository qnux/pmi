// Classe de gestion de la led multicolor
#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include "Arduino.h"

typedef enum {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	PURPLE,
	WHITE
} rgb_color_t;

typedef enum {
	OFF,
	ON,
	SLOW,
	FAST
} rgb_blink_t;


class RGB
{

public:
	RGB(int pin_r, int pin_g, int pin_b);

	void set(rgb_color_t color, rgb_blink_t blink);
	void off();

	void run();

private:

	void output(bool on);

	unsigned long m_timeChanged;
	rgb_color_t m_color;
	rgb_blink_t m_blink;
	int m_r;
	int m_g;
	unsigned int m_b;

};


#endif
// RGB_H
