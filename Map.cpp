#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "Map.h"

using namespace std;

// Retorna um limite de uma area dado um ponto de partida e uma direcao
int searchForBoundary(sf::Image img, int x, int y, char direction)
{
	int i, j;
	switch (direction)
	{
		case 'l':
			i = -1;
			j = 0;
			break;
		case 'r':
			i = 1;
			j = 0;
			break;
		case 'u':
			i = 0;
			j = -1;
			break;
		case 'd':
			i = 0;
			j = 1;
			break;
	}
	while (img.getPixel(x + i, y + j) == sf::Color::White)
	{
		x += i;
		y += j;
	}
	switch (direction)
	{
		case 'l':
		case 'r':
			return x + i;
		case 'u':
		case 'd':
			return y + j;
	}
}

// Retorna uma area usando a funcao searchForBoundary para encontrar todos os seus limites
Area findArea(sf::Image img, int x, int y, char direction)
{
	int m1, m2, w, h;
	sf::Vector2u size = img.getSize();
	w = size.x;
	h = size.y;
	Area area;
	switch (direction)
	{
		case 'l':
			area.left = searchForBoundary(img, x - 1, y, 'l');
			area.right = x;
			m1 = 0;
			m2 = h;
			for (int i = x - 1; img.getPixel(i, y) == sf::Color::White; i--)
			{
				m1 = max(m1, searchForBoundary(img, i, y, 'u'));
				m2 = min(m2, searchForBoundary(img, i, y, 'd'));
			}
			area.top = m1;
			area.bottom = m2;
			break;
		case 'r':
			area.right = searchForBoundary(img, x + 1, y, 'r');
			area.left = x;
			m1 = 0;
			m2 = h;
			for (int i = x + 1; img.getPixel(i, y) == sf::Color::White; i++)
			{
				m1 = max(m1, searchForBoundary(img, i, y, 'u'));
				m2 = min(m2, searchForBoundary(img, i, y, 'd'));
			}
			area.top = m1;
			area.bottom = m2;
			break;
		case 'u':
			area.top = searchForBoundary(img, x, y - 1, 'u');
			area.bottom = y;
			m1 = 0;
			m2 = w;
			for (int i = y - 1; img.getPixel(x, i) == sf::Color::White; i--)
			{
				m1 = max(m1, searchForBoundary(img, x, i, 'l'));
				m2 = min(m2, searchForBoundary(img, x, i, 'r'));
			}
			area.left = m1;
			area.right = m2;
			break;
		case 'd':
			area.bottom = searchForBoundary(img, x, y + 1, 'd');
			area.top = y;
			m1 = 0;
			m2 = w;
			for (int i = y + 1; img.getPixel(x, i) == sf::Color::White; i++)
			{
				m1 = max(m1, searchForBoundary(img, x, i, 'l'));
				m2 = min(m2, searchForBoundary(img, x, i, 'r'));
			}
			area.left = m1;
			area.right = m2;
			break;
	}
	return area;
}

// Metodo construtor. Encontra todas as areas e guarda as informacoes relevantes do mapa num arquivo, caso esse arquivo nao exista
Map::Map(string map_name)
{
	// O objetivo inicialmente nao pode estar completo
	this->goal_achieved = false;

	// Procura o arquivo ja salvo apenas com informacoes relevantes sobre as areas
	ifstream infile((map_name + ".md").c_str());
	if (infile.is_open())
	{
		int area_n, door_n, glass_n, patrol_n, guards_n, lights_n, j;
		infile >> area_n;
		for (int i = 0; i < area_n; i++)
		{
			Area area;
			infile >> area.left;
			infile >> area.right;
			infile >> area.top;
			infile >> area.bottom;
			for (j = 0; j < area_n; j++)
				area.intersect.push_back(false);
			infile >> j;
			while (j != -1)
			{
				area.intersect[j] = true;
				infile >> j;
			}
			this->areas.push_back(area);
		}
		infile >> this->starting_pos_x;
		infile >> this->starting_pos_y;
		for (int i = 0; i < area_n; i++)
		{
			infile >> j;
			this->pos_a.push_back(j);
		}
		infile >> door_n;
		for (int i = 0; i < door_n; i++)
		{
			int l, r, b;
			Area area;
			infile >> j;
			infile >> area.left;
			infile >> area.right;
			infile >> area.top;
			infile >> area.bottom;
			infile >> b;
			infile >> l;
			infile >> r;
			Door door(j, area, l, r, b);
			this->doors.push_back(door);
		}
		infile >> glass_n;
		for (int i = 0; i < glass_n; i++)
		{
			int n, a, b;
			infile >> n;
			infile >> a;
			infile >> b;
			Glass glass(n, a, b);
			this->glass.push_back(glass);
		}
		infile >> patrol_n;
		for (int i = 0; i < patrol_n; i++)
		{
			Area area;
			infile >> area.left;
			infile >> area.right;
			infile >> area.top;
			infile >> area.bottom;
			this->patrol.push_back(area);
		}
		infile >> guards_n;
		for (int i = 0; i < guards_n; i++)
		{
			int a, b, c, d, e;
			infile >> a;
			infile >> b;
			infile >> c;
			infile >> d;
			infile >> e;
			Guard guard(a, b, c, d, &this->patrol[e]);
			for (j = 0; j < door_n; j++)
				guard.addDoor(j, this->doors[j].getLeft(), this->doors[j].getY());
			this->guards.push_back(guard);
		}
		infile >> lights_n;
		for (int i = 0; i < lights_n; i++)
		{
			bool b;
			Area area;
			infile >> area.left;
			infile >> area.right;
			infile >> area.top;
			infile >> area.bottom;
			infile >> b;
			Light light(area, b);
			this->lights.push_back(light);
		}
		infile >> this->goal_pos_x;
		infile >> this->goal_pos_y;
		return;
	}

	// Caso o arquivo nao seja encontrado, cria-se um novo arquivo a partir da imagem
	sf::Image img;
	img.loadFromFile(map_name);
	sf::Vector2u size = img.getSize();
	int w = size.x - 1;
	int h = size.y - 1;
	this->goal_pos_x = -1;
	this->goal_pos_y = -1;
	this->starting_pos_x = -1;
	this->starting_pos_y = -1;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			// Encontra a posicao inicial do personagem
			if (img.getPixel(i, j) == sf::Color::Green)
			{
				// Deixa o pixel branco para nao atrapalhar na busca pelas areas
				img.setPixel(i, j, sf::Color::White);
				this->starting_pos_x = i;
				this->starting_pos_y = j;
				if (this->goal_pos_x >= 0)
					break;
			}
			// Encontra a posicao do objetivo
			else if (img.getPixel(i, j) == sf::Color::Yellow)
			{
				img.setPixel(i, j, sf::Color::White);
				this->goal_pos_x = i;
				this->goal_pos_y = j;
				if (this->starting_pos_x >= 0)
					break;
			}
		}
		if (this->starting_pos_x > 0)
			break;
	}

	// Encontra areas patrulhadas por guardas (e seus guardas)
	for (int i = 0; i < w; i++)
	{
		bool a, b, c, d, e, f;
		for (int j = 0; j < h; j++)
		{
			a = (img.getPixel(i,     j    ) == sf::Color::Red);
			b = (img.getPixel(i + 1, j    ) == sf::Color::Black);
			c = (img.getPixel(i,     j + 1) == sf::Color::Black);
			d = (img.getPixel(i + 1, j + 1) == sf::Color::Black);
			e = (img.getPixel(i,     j    ) == sf::Color::White);
			f = (img.getPixel(i,     j + 1) == sf::Color::Red);

			if (a && b && c && d)
			{
				img.setPixel(i, j, sf::Color::Black);
				Area area;
				int k = 0;
				while (img.getPixel(i + k, j) != sf::Color::Blue)
					k++;
				img.setPixel(i + k, j, sf::Color::Black);
				area.left = i;
				area.right = i + k;
				k = 0;
				while (img.getPixel(i, j - k) != sf::Color::Blue)
					k++;
				img.setPixel(i, j - k, sf::Color::Black);
				area.top = j - k;
				area.bottom = j - 2;
				this->patrol.push_back(area);
			}
			else if (e && f)
			{
				int k;
				for (k = 0; k < this->patrol.size(); k++)
				{
					if (i < this->patrol[k].left)
						continue;
					if (i > this->patrol[k].right)
						continue;
					if (j + 1 != this->patrol[k].bottom)
						continue;
					break;
				}
				if (img.getPixel(i - 1, j + 1) == sf::Color::Blue)
					a = true;
				else if (img.getPixel(i + 1, j + 1) == sf::Color::Blue)
					a = false;
				if (a && img.getPixel(i - 2, j + 1) == sf::Color::Blue)
					b = true;
				else if (!a && img.getPixel(i + 2, j + 1) == sf::Color::Blue)
					b = true;
				else
					b = false;
				Guard guard(i, j, b, a, &this->patrol[k]);
				this->guards.push_back(guard);
				for (k = 0; k < 5; k++)
					img.setPixel(i - 2 + k, j + 1, sf::Color::White);
			}
		}
	}

	// Encontra areas iluminadas (a luz pode ser desligada nessas areas)
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (img.getPixel(i, j) == sf::Color::Magenta)
			{
				Area area;
				area.left = i;
				area.top = j;
				int k = 0;
				while (img.getPixel(i + k, j) != sf::Color::Cyan)
					k++;
				area.right = i + k;
				k = 0;
				while (img.getPixel(i, j + k) != sf::Color::Cyan)
					k++;
				area.bottom = j + k;
				Light light(area, true);
				this->lights.push_back(light);
			}
		}
	}

	// Encontra todas as areas do mapa
	for (int i = 0; i < w; i++)
	{
		bool ab, bb, cb, db, aw, bw, cw, dw;
		for (int j = 0; j < h; j++)
		{
			// Encontra conjuntos de 2x2 pixels que configuram um 'canto'
			ab = (img.getPixel(i,     j    ) == sf::Color::Black); // superior esquerdo
			bb = (img.getPixel(i + 1, j    ) == sf::Color::Black); // superior direito
			cb = (img.getPixel(i,     j + 1) == sf::Color::Black); // inferior esquerdo
			db = (img.getPixel(i + 1, j + 1) == sf::Color::Black); // inferior direito
			aw = (img.getPixel(i,     j    ) == sf::Color::White); // superior esquerdo
			bw = (img.getPixel(i + 1, j    ) == sf::Color::White); // superior direito
			cw = (img.getPixel(i,     j + 1) == sf::Color::White); // inferior esquerdo
			dw = (img.getPixel(i + 1, j + 1) == sf::Color::White); // inferior direito

			// Superior esquerdo
			if (ab && bw && cw && dw)
			{
				this->areas.push_back(findArea(img, i, j, 'r'));
				this->areas.push_back(findArea(img, i, j, 'd'));
			}
			// Superior direito
			else if (aw && bb && cw && dw)
			{
				this->areas.push_back(findArea(img, i + 1, j, 'l'));
				this->areas.push_back(findArea(img, i + 1, j, 'd'));
			}
			// Inferior esquerdo
			else if (aw && bw && cb && dw)
			{
				this->areas.push_back(findArea(img, i, j + 1, 'r'));
				this->areas.push_back(findArea(img, i, j + 1, 'u'));
			}
			// Inferior direito
			else if (aw && bw && cw && db)
			{
				this->areas.push_back(findArea(img, i + 1, j + 1, 'l'));
				this->areas.push_back(findArea(img, i + 1, j + 1, 'u'));
			}
		}
	}

	// Encontra e apaga areas duplicadas
	for (int i = 0; i < this->areas.size(); i++)
	{
		bool l, r, t, b;
		for (int j = i + 1; j < this->areas.size(); j++)
		{
			l = (this->areas[i].left   == this->areas[j].left);
			r = (this->areas[i].right  == this->areas[j].right);
			t = (this->areas[i].top    == this->areas[j].top);
			b = (this->areas[i].bottom == this->areas[j].bottom);
			if (l && r && t && b)
				this->areas.erase(this->areas.begin() + j--);
		}
	}

	// Encontra areas delimitadas por portas
	for (int i = 0; i < w; i++)
	{
		bool a, b, c, d;
		for (int j = 0; j < h; j++)
		{
			a = (img.getPixel(i,     j    ) == sf::Color::Black);
			b = (img.getPixel(i + 1, j    ) == sf::Color::Black);
			c = (img.getPixel(i,     j + 1) == sf::Color::Red);
			d = (img.getPixel(i + 1, j + 1) == sf::Color::White);
			if (a && b && c && d)
			{
				int k = 0;
				while (img.getPixel(i - k, j + 1) == sf::Color::Red)
					k++;
				Area a1 = findArea(img, i - k, j + 1, 'd');
				Area a2 = findArea(img, i + 1, j + 1, 'd');
				k = a1.right;
				a1.right = a2.right;
				a1.top = j;
				this->areas.push_back(a1);
				a2.right = a2.left + 1;
				a2.left = k - 1;
				k = 0;
				while (img.getPixel(i - k, j) == sf::Color::Black)
					k++;
				a1.left = i - k;
				k = 0;
				while (img.getPixel(i + k, j) == sf::Color::Black)
					k++;
				a1.right = i + k;
				Door door(this->areas.size() - 1, a1, a2.left, a2.right, false);
				this->doors.push_back(door);
				for (k = 0; k < this->guards.size(); k++)
					this->guards[k].addDoor(this->doors.size() - 1, a2.left, a2.bottom);
			}
		}
	}

	// Encontra areas delimitadas por janelas
	int area_n = this->areas.size();
	for (int i = 0; i < w; i++)
	{
		bool a, c, d;
		for (int j = 0; j < h; j++)
		{
			a = (img.getPixel(i,     j    ) == sf::Color::Black);
			c = (img.getPixel(i,     j + 1) == sf::Color::Blue);
			d = (img.getPixel(i + 1, j + 1) == sf::Color::White);
			if (a && c && d)
			{
				int left = 0;
				int right = w;
				int k = 0;
				while (img.getPixel(i - k, j + 1) == sf::Color::Blue)
					k++;
				vector<bool> b_left;
				vector<bool> b_right;
				for (int l = 0; l < area_n; l++)
				{
					b_left.push_back(true);
					b_right.push_back(true);
				}
				int l;
				for (l = 1; img.getPixel(i, j + l) == sf::Color::Blue; l++)
				{
					left = max(left, searchForBoundary(img, i - k, j + l, 'l'));
					right = min(right, searchForBoundary(img, i + 1, j + l, 'r'));
					vector<bool> compare_left = this->getPossibleAreas(i - k, j + l, area_n);
					vector<bool> compare_right = this->getPossibleAreas(i + 1, j + l, area_n);
					for (int m = 0; m < area_n; m++)
					{
						if (!compare_left[m])
							b_left[m] = false;
						if (!compare_right[m])
							b_right[m] = false;
					}
				}
				Area area;
				area.left = left;
				area.right = right;
				area.top = j;
				area.bottom = j + l;
				this->areas.push_back(area);
				for (l = 0; l < area_n; l++)
				{
					if (b_left[l])
						left = l;
					if (b_right[l])
						right = l;
				}
				Glass glass(this->areas.size() - 1, left, right);
				this->glass.push_back(glass);
			}
		}
	}

	// Atualiza o tamanho do vetor de intersecoes e inicializa seus elementos como false
	for (int i = 0; i < this->areas.size(); i++)
		for (int j = 0; j < this->areas.size(); j++)
			this->areas[i].intersect.push_back(false);

	// Encontra e registra intersecoes
	for (int i = 0; i < this->areas.size(); i++)
	{
		int a, b, c, d, e, f, g, h;
		for (int j = i + 1; j < this->areas.size(); j++)
		{
			a = this->areas[i].left;
			b = this->areas[i].right;
			c = this->areas[j].left;
			d = this->areas[j].right;
			e = this->areas[i].top;
			f = this->areas[i].bottom;
			g = this->areas[j].top;
			h = this->areas[j].bottom;
			if (!(a < b && c < d && (b < c || d < a)) && !(e < f && g < h && (f < g || h < e)))
			{
				this->areas[i].intersect[j] = true;
				this->areas[j].intersect[i] = true;
			}
		}
	}

	// Calcula as areas nas quais o personagem esta inserido a partir da posicao inicial
	for (int i = 0; i < this->areas.size(); i++)
	{
		bool a, b, c, d;
		a = (this->starting_pos_x > this->areas[i].left);
		b = (this->starting_pos_x < this->areas[i].right);
		c = (this->starting_pos_y > this->areas[i].top);
		d = (this->starting_pos_y < this->areas[i].bottom);
		if (a && b && c && d)
			this->pos_a.push_back(true);
		else
			this->pos_a.push_back(false);
	}

	// Guarda as informacoes no arquivo
	ofstream outfile((map_name + ".md").c_str());
	outfile << this->areas.size() << endl;
	for (int i = 0; i < this->areas.size(); i++)
	{
		outfile << this->areas[i].left << " ";
		outfile << this->areas[i].right << " ";
		outfile << this->areas[i].top << " ";
		outfile << this->areas[i].bottom << " ";
		for (int j = 0; j < this->areas.size(); j++)
			if (this->areas[i].intersect[j])
				outfile << j << " ";
		outfile << -1 << endl;
	}
	outfile << this->starting_pos_x << " ";
	outfile << this->starting_pos_y << endl;
	for (int i = 0; i < this->areas.size(); i++)
		outfile << this->pos_a[i] << " ";
	outfile << endl << this->doors.size() << endl;
	for (int i = 0; i < this->doors.size(); i++)
	{
		Area area = this->doors[i].getClosedArea();
		outfile << this->doors[i].getArea() << " ";
		outfile << area.left << " ";
		outfile << area.right << " ";
		outfile << area.top << " ";
		outfile << area.bottom << " ";
		outfile << false << " ";
		outfile << this->doors[i].getLeft() << " ";
		outfile << this->doors[i].getRight() << endl;
	}
	outfile << this->glass.size() << endl;
	for (int i = 0; i < this->glass.size(); i++)
	{
		outfile << this->glass[i].getArea() << " ";
		outfile << this->glass[i].getAreaA() << " ";
		outfile << this->glass[i].getAreaB() << endl;
	}
	outfile << this->patrol.size() << endl;
	for (int i = 0; i < this->patrol.size(); i++)
	{
		outfile << this->patrol[i].left << " ";
		outfile << this->patrol[i].right << " ";
		outfile << this->patrol[i].top << " ";
		outfile << this->patrol[i].bottom << endl;
	}
	outfile << this->guards.size() << endl;
	for (int i = 0; i < this->guards.size(); i++)
	{
		int j;
		outfile << this->guards[i].getPosX() << " ";
		outfile << this->guards[i].getPosY() << " ";
		outfile << this->guards[i].isWalking() << " ";
		outfile << this->guards[i].getFacingDirection() << " ";
		for (j = 0; j < this->patrol.size(); j++)
			if (this->guards[i].getAreaPtr() == &this->patrol[j])
				break;
		outfile << j << endl;
	}
	outfile << this->lights.size() << endl;
	for (int i = 0; i < this->lights.size(); i++)
	{
		outfile << this->lights[i].getArea().left << " ";
		outfile << this->lights[i].getArea().right << " ";
		outfile << this->lights[i].getArea().top << " ";
		outfile << this->lights[i].getArea().bottom << " ";
		outfile << this->lights[i].isOn() << endl;
	}
	outfile << this->goal_pos_x << " ";
	outfile << this->goal_pos_y << endl;
}

Area Map::getArea(int n)
{
	return this->areas[n];
}

bool Map::getPosA(int n)
{
	return this->pos_a[n];
}

vector<bool> Map::getPosA(void)
{
	return this->pos_a;
}

void Map::setPosA(vector<bool> pos)
{
	this->pos_a = pos;
}

int Map::getNumberOfAreas(void)
{
	return this->areas.size();
}

int Map::getStartingPosX(void)
{
	return this->starting_pos_x;
}

int Map::getStartingPosY(void)
{

	return this->starting_pos_y;
}

bool Map::passable(int x, int y, bool ignore_glass)
{
	for (int i = 0; i < this->areas.size(); i++)
	{
		if (this->pos_a[i])
		{
			for (int j = 0; j < this->doors.size(); j++)
				if (this->doors[j].getArea() == i)
					if (!this->doors[j].isOpen())
						return false;
			if (!ignore_glass)
				for (int j = 0; j < this->glass.size(); j++)
					if (this->glass[j].getArea() == i)
						if (!this->glass[j].isBroken())
							return false;
			if (x >= this->areas[i].left)
				if (x <= this->areas[i].right)
					if (y >= this->areas[i].top)
						if (y <= this->areas[i].bottom)
							return true;
		}
	}
	return false;
}

void Map::updatePosA(int x, int y)
{
	for (int i = 0; i < this->areas.size(); i++)
	{
		if (this->pos_a[i])
		{
			bool b = false;
			for (int j = 0; j < this->areas[i].intersect.size(); j++)
			{
				if (this->areas[i].intersect[j] && !this->pos_a[j])
				{
					this->pos_a[j] = true;
					b = true;
				}
			}
			if (b)
				break;
		}
	}
	for (int i = 0; i < this->areas.size(); i++)
		if (this->pos_a[i])
			if (x < this->areas[i].left || x > this->areas[i].right || y < this->areas[i].top || y > this->areas[i].bottom)
				this->pos_a[i] = false;
}

Step Map::step(int mx, int my, int nx, int ny)
{
	int ax, ay;
	Step step;
	nx -= mx;
	ny -= my;
	if (nx == 0)
		ax = 0;
	else
		ax = 2 * nx / abs(nx);
	if (ny == 0)
		ay = 0;
	else
		ay = 2 * ny / abs(ny);
	for (int i = 0; i < this->areas.size(); i++)
	{
		if (!this->pos_a[i])
			continue;
		int j;
		for (j = 0; j < this->doors.size(); j++)
			if (this->doors[j].getArea() == i && !this->doors[j].isOpen())
				break;
		if (j < this->doors.size())
			continue;
		for (j = 0; j < this->glass.size(); j++)
		{
			this->glass[j].bump(this->pos_a, this->getPossibleAreas(mx + nx, my + ny, this->areas.size()), nx, ny, true);
			if (this->glass[j].getArea() == i && !this->glass[j].isBroken())
				break;
		}
		if (j < this->glass.size())
			continue;
		step.d = 'e';
		if (mx == this->areas[i].left + 1 && nx <= 0)
			step.d = 'l';
		else if (mx == this->areas[i].right - 1 && nx >= 0)
			step.d = 'r';
		else if (my == this->areas[i].top + 1 && ny <= 0)
			step.d = 'u';
		else if (my == this->areas[i].bottom - 1 && ny >= 0)
			step.d = 'd';
		if (step.d != 'e')
			continue;
		step.x = -1;
		step.y = -1;
		if (this->areas[i].left > mx + nx)
			step.x = this->areas[i].left + 1;
		else if (this->areas[i].right < mx + nx)
			step.x = this->areas[i].right - 1;
		if (this->areas[i].top > my + ny)
			step.y = this->areas[i].top + 1;
		else if (this->areas[i].bottom < my + ny)
			step.y = this->areas[i].bottom - 1;
		if (step.x != -1 || step.y != -1)
		{
			if (step.x != -1)
			{
				step.x -= mx;
				step.y = (int) (ny * ((float) step.x) / nx);
				if (ax < 0)
					step.d = 'l';
				else
					step.d = 'r';
			}
			else
			{
				step.y -= my;
				step.x = (int) (nx * ((float) step.y) / ny);
				if (ay < 0)
					step.d = 'u';
				else
					step.d = 'd';
			}
			step.x += mx;
			step.y += my;
			if (this->passable(step.x + ax, step.y + ay, false))
				continue;
			if (this->passable(step.x, step.y, false))
				return step;
		}
		else
		{
			step.x = mx + nx;
			step.y = my + ny;
			step.d = 'n';
			return step;
		}
	}
	step.x = mx;
	step.y = my;
	return step;
}

bool Map::bumpAll(int x, int y)
{
	for (int i = 0; i < this->doors.size(); i++)
		if (this->doors[i].bump(x, y))
			return true;
	return false;
}

bool Map::bumpGlass(int xa, int ya, int xb, int yb, bool real)
{
	for (int i = 0; i < this->glass.size(); i++)
		if (this->glass[i].bump(this->pos_a, this->getPossibleAreas(xb, yb, this->areas.size()), xb - xa, yb - ya, real))
			return true;
	return false;
}

vector<bool> Map::getPossibleAreas(int x, int y, int n)
{
	vector<bool> r;
	for (int i = 0; i < n; i++)
	{
		bool b = true;
		if (x < this->areas[i].left)
			b = false;
		if (x > this->areas[i].right)
			b = false;
		if (y < this->areas[i].top)
			b = false;
		if (y > this->areas[i].bottom)
			b = false;
		r.push_back(b);
	}
	return r;
}

bool Map::guardsMove(int x, int y, int speed, bool dead)
{
	for (int i = 0; i < guards.size(); i++)
	{
		if (!dead)
		{
			vector<bool> old_pos_a = this->pos_a;
			int j, dx, dy, steps = 5;
			for (j = steps; j > 0; j--)
			{
				dx = (x - this->guards[i].getPosX()) / j;
				dy = (y - this->guards[i].getPosY()) / j;
				if (this->guards[i].isDistracted() && abs(dy) > abs(dx) + 30)
					break;
				if (this->guards[i].getFacingDirection() && dx > 0)
					break;
				if (!this->guards[i].getFacingDirection() && dx < 0)
					break;
				if (this->passable(x - dx, y - dy, true))
				{
					if (j == 1 && this->guards[i].isInRange(x, y))
						this->bumpGlass(x - dx, y - dy, x, y, true);
					this->updatePosA(x - dx, y - dy);
				}
				else
					break;
			}
			this->pos_a = old_pos_a;
			if (j == 0)
			{
				for (; j < this->lights.size(); j++)
					if (this->lights[i].isInDarkArea(x, y))
						break;
				dead = this->guards[i].shoot(x, y, speed, dead, !(j < this->lights.size()));
			}
		}
		vector<int> v;
		for (int j = 0; j < this->guards[i].getDoorVectorSize(); j++)
		{
			if (!this->doors[this->guards[i].getDoor(j)].isOpen())
			{
				v.push_back(this->doors[this->guards[i].getDoor(j)].getLeft());
				v.push_back(this->doors[this->guards[i].getDoor(j)].getRight());
			}
		}
		this->guards[i].move(v);
	}
	return dead;
}

Guard Map::getGuard(int n)
{
	return this->guards[n];
}

int Map::getNumberOfGuards(void)
{
	return this->guards.size();
}

int Map::getGuardX(int n)
{
	return this->guards[n].getPosX();
}

int Map::getGuardY(int n)
{
	return this->guards[n].getPosY();
}

int Map::guardAnim(int n)
{
	return this->guards[n].getAnimFrame();
}

int Map::getGoalX(void)
{
	return this->goal_pos_x;
}

int Map::getGoalY(void)
{
	return this->goal_pos_y;
}

void Map::finish(void)
{
	this->goal_achieved = true;
}

bool Map::done(void)
{
	return this->goal_achieved;
}

