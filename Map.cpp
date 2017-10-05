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
	area.pass = true;
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
	// Procura o arquivo ja salvo apenas com informacoes relevantes sobre as areas
	ifstream infile((map_name + ".md").c_str());
	if (infile.is_open())
	{
		int area_n, j;
		infile >> area_n;
		for (int i = 0; i < area_n; i++)
		{
			Area area;
			infile >> area.left;
			infile >> area.right;
			infile >> area.top;
			infile >> area.bottom;
			infile >> area.pass;
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
		return;
	}

	// Caso o arquivo nao seja encontrado, cria-se um novo arquivo a partir da imagem
	sf::Image img;
	img.loadFromFile(map_name);
	sf::Vector2u size = img.getSize();
	int w = size.x - 1;
	int h = size.y - 1;
	this->starting_pos_x = 0;
	this->starting_pos_y = 0;
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
				break;
			}
		}
		if (this->starting_pos_x > 0)
			break;
	}
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
				int k;
				while (img.getPixel(i - k, j + 1) == sf::Color::Red)
					k++;
				Area a1 = findArea(img, i - k, j + 1, 'd');
				Area a2 = findArea(img, i + 1, j + 1, 'd');
				a1.right = a2.right;
				a1.top = j;
				a1.pass = false;
				this->areas.push_back(a1);
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
		cout << this->areas[i].left << "~" << this->starting_pos_x << "~" << this->areas[i].right << endl;
		cout << this->areas[i].top << "~" << this->starting_pos_y << "~" << this->areas[i].bottom << endl << endl;
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
		outfile << this->areas[i].pass << " ";
		for (int j = 0; j < this->areas.size(); j++)
			if (this->areas[i].intersect[j])
				outfile << j << " ";
		outfile << -1 << endl;
	}
	outfile << this->starting_pos_x << " ";
	outfile << this->starting_pos_y << endl;
	for (int i = 0; i < this->areas.size(); i++)
		outfile << this->pos_a[i] << " ";
	outfile << endl;
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

bool Map::passable(int x, int y)
{
	for (int i = 0; i < this->areas.size(); i++)
		if (this->pos_a[i] && this->areas[i].pass)
			if (x >= this->areas[i].left)
				if (x <= this->areas[i].right)
					if (y >= this->areas[i].top)
						if (y <= this->areas[i].bottom)
							return true;
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
/*
			{
				i = 0;
				continue;
			}
*/
		}
	}
	for (int i = 0; i < this->areas.size(); i++)
		if (this->pos_a[i])
			if (x < this->areas[i].left || x > this->areas[i].right || y < this->areas[i].top || y > this->areas[i].bottom)
				this->pos_a[i] = false;
}

Step Map::step(int mx, int my, int nx, int ny)
{
	int tx, ty, ax, ay;
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
			if (this->passable(step.x + ax, step.y + ay))
				continue;
			if (this->passable(step.x, step.y))
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
	step.d = 'e';
	return step;
}

