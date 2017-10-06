#include "Door.h"

Door::Door(int n, Area area)
{
	this->area = n;
	this->open = false;
	this->area_closed = area;
}

int Door::getArea(void)
{
	return this->area;
}

Area Door::getClosedArea(void)
{
	Area area;
	area.left = this->area_closed.left;
	area.right = this->area_closed.right;
	area.top = this->area_closed.top;
	area.bottom = this->area_closed.bottom;
	return area;
}

bool Door::bump(int x, int y)
{
	if (this->open)
		return false;
	if (x < this->area_closed.left)
		return false;
	if (x > this->area_closed.right)
		return false;
	if (y < this->area_closed.top)
		return false;
	if (y > this->area_closed.bottom)
		return false;
	return true;
}

bool Door::isOpen(void)
{
	return this->open;
}

void Door::toggle(void)
{
	this->open = !this->open;
}

