#include "Light.h"
#include "Area.h"

Light::Light(Area area, bool on)
{
	this->area = area;
	this->on = on;
}

Area Light::getArea(void)
{
	return this->area;
}

void Light::toggle(void)
{
	this->on = !this->on;
}

bool Light::isOn(void)
{
	return this->on;
}

bool Light::isInDarkArea(int x, int y)
{
	if (this->on)
		return false;
	if (x < this->area.left)
		return false;
	if (x > this->area.right)
		return false;
	if (y < this->area.top)
		return false;
	if (y > this->area.bottom)
		return false;
	return true;
}

