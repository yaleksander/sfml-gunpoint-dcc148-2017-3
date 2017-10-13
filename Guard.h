#ifndef GUARD_H
#define GUARD_H

#include <vector>
#include "Area.h"

using namespace std;

class Guard
{
	private:

		int anim;
		int pos_x;
		int pos_y;
		int attention;
		int wait;
		bool walk;
		bool left;
		bool reloading;
		Area* patrol;
		vector<int> doors;

	public:

		Guard(int x, int y, bool w, bool l, Area* a);
		void addDoor(int n, int x, int y);
		int getNumberOfDoors(void);
		void startMovingIf(bool condition);
		void move(vector<int> limits);
		void setAttention(int x);
		bool shoot(int x, int y, int speed, bool b, bool light);
		bool isInRange(int x, int y);
		int getPosX(void);
		int getPosY(void);
		int getDoor(int n);
		int getDoorVectorSize(void);
		bool getFacingDirection(void);
		bool isWalking(void);
		bool isDistracted(void);
		Area* getAreaPtr(void);
		int getAnimFrame(void);
};

#endif

