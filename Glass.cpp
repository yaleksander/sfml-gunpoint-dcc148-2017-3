#include <iostream>
#include "Glass.h"

#define GLASS_RESISTANCE 175

using namespace std;

Glass::Glass(int n, int a, int b)
{
	this->whole_area = n;
	this->area_a = a;
	this->area_b = b;
	this->broken = false;
}

int Glass::getArea(void)
{
	return this->whole_area;
}

int Glass::getAreaA(void)
{
	return this->area_a;
}

int Glass::getAreaB(void)
{
	return this->area_b;
}

bool Glass::isNear(int n)
{
	if (this->area_a == n || this->area_b == n)
		return true;
	else
		return false;
}

bool Glass::bump(vector<bool> area_a, vector<bool> area_b, int fx, int fy, bool real)
{
	int a, b, i;
	for (i = 0; i < area_a.size(); i++)
		if (area_a[i] && i == this->area_a || i == this->area_b)
			break;
	if (i < area_a.size())
		a = i;
	else
		a = -1;
	for (i = 0; i < area_b.size(); i++)
		if (area_b[i] && i == this->area_a || i == this->area_b)
			break;
	if (i < area_b.size())
		b = i;
	else
		b = -1;
	if (a == -1 || b == -1 || a == b)
		return false;
	if (fx * fx + fy * fy > GLASS_RESISTANCE)
	{
		if (real)
			this->broken = true;
		return true;
	}
	return false;
}

bool Glass::isBroken(void)
{
	return this->broken;
}

