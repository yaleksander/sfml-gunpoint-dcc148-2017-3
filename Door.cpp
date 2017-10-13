#include "Door.h"

Door::Door(int n, Area area, int l, int r, bool b)
{
	this->area = n;
	this->open = false;
	this->area_closed = area;
	this->xa = l;
	this->xb = r;
	this->open = b;
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

void Door::setOpenTo(char c)
{
	if (c == 'l')
		this->left = true;
	else if (c == 'r')
		this->left = false;
}

void Door::toggle(void)
{
	this->open = !this->open;
}

int Door::getLeft(void)
{
	return this->xa;
}

int Door::getRight(void)
{
	return this->xb;
}

int Door::getY(void)
{
	return this->area_closed.bottom;
}

