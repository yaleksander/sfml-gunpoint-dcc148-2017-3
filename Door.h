#ifndef DOOR_H
#define DOOR_H

#include <vector>

using namespace std;

typedef struct Area Area;

struct Area
{
	// limites
	int left;
	int right;
	int top;
	int bottom;

	// passabilidade
	bool pass;

	// intersecoes
	vector<bool> intersect;
};

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
