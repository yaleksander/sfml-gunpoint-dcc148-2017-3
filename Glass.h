#ifndef GLASS_H
#define GLASS_H

#include <vector>

using namespace std;

class Glass
{
	private:

		int whole_area;
		int area_a;
		int area_b;
		bool broken;

	public:

		Glass(int n, int a, int b);
		int getArea(void);
		int getAreaA(void);
		int getAreaB(void);
		bool isNear(int n);
		bool isBroken(void);
		bool bump(vector<bool> area_a, vector<bool> area_b, int fx, int fy, bool real);
};

#endif

