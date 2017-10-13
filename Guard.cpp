#include <iostream>
#include <cmath>
#include "Guard.h"

#define FRAME_WAIT 4
#define WAIT 180
#define TURN_A 80
#define TURN_B 120
#define HEAR 800
#define VIEW 500
#define DARK 75
#define SHOOT 100
#define RELOAD 60
#define WALK 1
#define RUN 3
#define ERROR 150
#define WIDTH 10

using namespace std;

Guard::Guard(int x, int y, bool w, bool l, Area* a)
{
	this->pos_x = x;
	this->pos_y = y;
	this->walk = w;
	this->left = l;
	this->patrol = a;
	this->attention = -1;
	this->wait = -1;
	this->reloading = false;
}

void Guard::addDoor(int n, int x, int y)
{
	if (x >= this->patrol->left && x <= this->patrol->right && y == this->patrol->bottom + 1)
		this->doors.push_back(n);
}

int Guard::getNumberOfDoors(void)
{
	return this->doors.size();
}

void Guard::startMovingIf(bool condition)
{
	if (condition)
		this->walk = true;
}

void Guard::move(vector<int> limits)
{
	if (this->walk)
		this->anim++;
	int llimit = this->patrol->left;
	int rlimit = this->patrol->right;
	for (int i = 0; i < limits.size(); i++)
	{
		if (limits[i] < pos_x && limits[i] > llimit)
			llimit = limits[i];
		if (limits[i] > pos_x && limits[i] < rlimit)
			rlimit = limits[i];
	}
	llimit += WIDTH;
	rlimit -= WIDTH;
	if (this->wait == -1 && this->walk)
	{
		if (this->attention == -1)
		{
			if (this->anim >= 8 * FRAME_WAIT)
				this->anim = 0;
			if (this->left)
				this->pos_x = max(this->pos_x - WALK, llimit);
			else
				this->pos_x = min(this->pos_x + WALK, rlimit);
			if (this->pos_x == llimit || this->pos_x == rlimit)
				this->left = !this->left;
		}
		else
		{
			if (this->anim >= 4 * FRAME_WAIT)
				this->anim = 0;
			if (attention < this->pos_x)
			{
				int m = max(attention, llimit);
				this->left = true;
				this->pos_x = max(this->pos_x - RUN, m);
			}
			else if (attention > this->pos_x)
			{
				int m = min(attention, rlimit);
				this->left = false;
				this->pos_x = min(this->pos_x + RUN, m);
			}
			if (this->pos_x == attention || this->pos_x == llimit || this->pos_x == rlimit)
				this->setAttention(-1);
		}
	}
	else
	{
		this->wait++;
		if (this->wait == WAIT || (this->reloading && this->wait == RELOAD))
		{
			this->wait = -1;
			this->reloading = false;
			this->anim = 0;
		}
		else if (this->wait == TURN_A || this->wait == TURN_B)
			this->left = !this->left;
	}
}

void Guard::setAttention(int x)
{
	if (this->attention == -1)
		this->anim = 0;
	this->attention = x;
	if (x >= 0)
	{
		this->walk = true;
		this->wait = -1;
	}
	else
		this->wait = 0;
}

bool Guard::shoot(int x, int y, int speed, bool b, bool light)
{
	if (this->wait >= 0 || b)
		return false;
	int dx = this->pos_x - x;
	int dy = this->pos_y - y;
	int dist = sqrt(dx * dx + dy * dy);
	if (dist < (light ? VIEW : DARK) && dist < SHOOT)
	{
		this->reloading = true;
		this->setAttention(-1);
		return (speed < ERROR);
	}
	else if (dist < (light ? VIEW : DARK))
	{
		this->setAttention(x);
		return false;
	}
	else
		return false;
}

bool Guard::isInRange(int x, int y)
{
	int dx = x - this->pos_x;
	int dy = y - this->pos_y;
	return (sqrt(dx * dx + dy * dy) < SHOOT);
}

int Guard::getPosX(void)
{
	return this->pos_x;
}

int Guard::getPosY(void)
{
	return this->pos_y;
}

int Guard::getDoor(int n)
{
	return this->doors[n];
}

int Guard::getDoorVectorSize(void)
{
	return this->doors.size();
}

bool Guard::getFacingDirection(void)
{
	return this->left;
}

bool Guard::isWalking(void)
{
	return this->walk;
}

bool Guard::isDistracted(void)
{
	return (this->attention == -1);
}

Area* Guard::getAreaPtr(void)
{
	return this->patrol;
}

int Guard::getAnimFrame(void)
{
	if (this->wait != -1)
		return this->left ? 157 : 57;
	else if (!this->reloading)
	{
		if (!this->walk)
			return this->left ? 156 : 56;
		else if (this->attention == -1)
			return (this->left ? 148 : 48) + this->anim / FRAME_WAIT;
		else
			return (this->left ? 160 : 60) + this->anim / FRAME_WAIT;
	}
	else if (this->anim >= 2 * FRAME_WAIT)
		return this->left ? 157 : 57;
	else
		return (this->left ? 158 : 58) + this->anim / FRAME_WAIT;
}

