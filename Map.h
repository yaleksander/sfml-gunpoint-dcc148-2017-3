#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include "Area.h"
#include "Door.h"
#include "Glass.h"
#include "Guard.h"
#include "Light.h"

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

		int goal_pos_x;
		int goal_pos_y;
		bool goal_achieved;
		int starting_pos_x;
		int starting_pos_y;
		vector<bool> pos_a;
		vector<Area> areas;
		vector<Area> patrol;
		vector<Door> doors;
		vector<Glass> glass;
		vector<Guard> guards;
		vector<Light> lights;

	public:

		Map(string map_name);
		Area getArea(int n);
		bool getPosA(int n);
		vector<bool> getPosA(void);
		void setPosA(vector<bool> pos);
		int getNumberOfAreas(void);
		int getStartingPosX(void);
		int getStartingPosY(void);
		bool passable(int x, int y, bool ignore_glass);
		void updatePosA(int x, int y);
		Step step(int mx, int my, int nx, int ny);
		bool bumpAll(int x, int y);
		bool bumpGlass(int xa, int ya, int xb, int yb, bool real);
		vector<bool> getPossibleAreas(int x, int y, int n);
		bool guardsMove(int x, int y, int speed, bool dead);
		Guard getGuard(int n);
		int getNumberOfGuards(void);
		int getGuardX(int n);
		int getGuardY(int n);
		int guardAnim(int n);
		int getGoalX(void);
		int getGoalY(void);
		void finish(void);
		bool done(void);
};

#endif
