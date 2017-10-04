#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

using namespace std;

typedef struct Area Area;
typedef struct Step Step;

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

struct Step
{
	int x;
	int y;
	char d;
};

class Map
{
	private:

		vector<Area> areas;
		vector<bool> pos_a;
		int starting_pos_x;
		int starting_pos_y;

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
};

#endif