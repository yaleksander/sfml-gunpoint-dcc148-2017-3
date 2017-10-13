#ifndef AREA_H
#define AREA_H

#include <vector>

using namespace std;

typedef struct Area Area;

struct Area
{
	int left;
	int right;
	int top;
	int bottom;
	vector<bool> intersect;
};

#endif
