#ifndef DOOR_H
#define DOOR_H

#include "Area.h"

class Door
{
	private:

		int xa;
		int xb;
		int area;
		bool open;
		bool left;
		Area area_closed;

	public:

		Door(int n, Area area, int l, int r, bool b);
		int getArea(void);
		Area getClosedArea(void);
		bool bump(int x, int y);
		bool isOpen(void);
		void setOpenTo(char c);
		void toggle(void);
		int getLeft(void);
		int getRight(void);
		int getY(void);
};

#endif

