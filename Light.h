#ifndef LIGHT_H
#define LIGHT_H

#include "Area.h"

class Light
{
	private:

		Area area;
		bool on;

	public:

		Light(Area area, bool on);
		Area getArea(void);
		void toggle(void);
		bool isOn(void);
		bool isInDarkArea(int x, int y);
};

#endif
