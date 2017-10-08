#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include "Area.h"
#include "Door.h"
#include "Glass.h"

using namespace std;

typedef struct Step Step;

struct Step
{
	int x;
	int y;
	char d;
};

class Map
{
	private:

		int starting_pos_x;
		int starting_pos_y;
		vector<bool> pos_a;
		vector<Area> areas;
		vector<Door> doors;
		vector<Glass> glass;

	public:

		Map(string map_name);
		Area getArea(int n);
		bool getPosA(int n);
		vector<bool> getPosA(void);
		void setPosA(vector<bool> pos);
		int getNumberOfAreas(void);
		int getStartingPosX(void);
		int getStartingPosY(void);
		bool passable(int x, int y);
		void updatePosA(int x, int y);
		Step step(int mx, int my, int nx, int ny);
		bool bumpAll(int x, int y);
		bool bumpGlass(int xa, int ya, int xb, int yb);
		vector<bool> getPossibleAreas(int x, int y, int n);
};

#endif
