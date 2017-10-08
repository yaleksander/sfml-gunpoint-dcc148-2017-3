#ifndef DOOR_H
#define DOOR_H

#include "Area.h"

class Door
{
	private:

		int area;
		bool open;
		Area area_closed;

	public:

		Door(int n, Area area);
		int getArea(void);
		Area getClosedArea(void);
		bool bump(int x, int y);
		bool isOpen(void);
		void toggle(void);
};

#endif

