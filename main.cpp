#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <sys/time.h>
#include "Map.h"

using namespace std;

// Informacoes basicas
string settings     = "settings.txt";
string map_name     = "map4.png";
string window_title = "Gunpoint Prototype";
string font_path    = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf";

// Controle de resolucao e fps
int window_width = 1024;
int window_height = 728;
int fps;
int maxFps = 120;

// Controle de movimento e animacao do personagem
float x;
float y;
float vx;
float vy;
int anim_frame_wait = 8;
int adjust_x;
int adjust_y;
int max_ground_speed = 4;
int max_wall_speed = 2;
int take_off_speed = 30;
int jump_strength;
int jump_strength_calc;
int jump_strength_start = 250;
int jump_strength_max = 600;
int player_size_x = 10;
int player_size_y = 15;
int player_start_x;
int player_start_y;
int player_map_x;
int player_map_y;
int player_screen_x;
int player_screen_y;
int player_fake_pos_x;
int player_fake_pos_y;
int player_w;
int player_h;
char player_d;
bool player_facing_left;
bool run_into_wall;
bool landing;
bool dead = false;

// Distancia maxima pro jogador interagir com um objeto do jogo
int button_dist_max = 20;

// Variavel com todos os sprites do jogo
vector<sf::Texture> all_textures;

// Controle de teclas pressionadas
bool trigger_manager = true;
bool button_trigger_manager = true;

// Controle das areas nas quais o personagem esta inserido
vector<bool> player_a;
Area area;

// Dados relevantes do mapa da fase
Map level_map(map_name);

// Constantes
float ati = 0.04;	// valor somado a vy em cada iteracao
int itr = 5;            // desenha um ponto do trajeto do pulo a cada itr iteracoes
int ji = 3;      	// valor somado a jump_strength em cada iteracao
float k0 = 0.7;  	// constante multiplicadora de jump_strength
float k1 = 90.0; 	// aceleracao gravitacional do jogo
float k2 = 150.0; 	// modulo do vetor direcao
float k3 = 4.0; 	// magica

// Contador (pra nao precisar inicializar o mesmo contador varias vezes)
int i;

// Funcao para adicionar mais texturas a variavel com todas as texturas
void addTexture(string filename, int cols, int rows, bool invert)
{
	sf::Image spriteset;
	sf::Texture tex;
	spriteset.loadFromFile(filename);
	int w = spriteset.getSize().x;
	int h = spriteset.getSize().y;
	if (invert)
		spriteset.flipHorizontally();
	for (i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tex.loadFromImage(spriteset, sf::IntRect(invert ? (w - (j + 1) * w / cols) : (j * w / cols), i * h / rows, w / cols, h / rows));
			all_textures.push_back(sf::Texture(tex));
		}
	}
}

int main(void)
{
	int window_ini_x = (sf::VideoMode::getDesktopMode().width - window_width) / 2;
	int window_ini_y = (sf::VideoMode::getDesktopMode().height - window_height) / 2;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), window_title, sf::Style::Close);
	window.setPosition(sf::Vector2i(window_ini_x, window_ini_y));
	sf::View viewport;

	sf::Texture texture;
	texture.loadFromFile(map_name);

	addTexture("sprites/player.png", 9, 3, false);
	addTexture("sprites/player_jump.png", 9, 1, false);
	addTexture("sprites/player_die.png", 5, 1, false);
	addTexture("sprites/player_punch_guard.png", 7, 1, false);
	addTexture("sprites/guard.png", 8, 2, false);
	addTexture("sprites/player_hack.png", 6, 6, false);
	addTexture("sprites/player.png", 9, 3, true);
	addTexture("sprites/player_jump.png", 9, 1, true);
	addTexture("sprites/player_die.png", 5, 1, true);
	addTexture("sprites/player_punch_guard.png", 7, 1, true);
	addTexture("sprites/guard.png", 8, 2, true);
	addTexture("sprites/player_hack.png", 6, 6, true);

	adjust_x = (window_width - texture.getSize().x) / 2;
	adjust_y = window_height - texture.getSize().y;

	sf::RectangleShape shape;
	shape.setPosition(adjust_x, adjust_y);
	shape.setSize(sf::Vector2f(texture.getSize().x, texture.getSize().y));
	shape.setTexture(&texture);

	player_start_x = level_map.getStartingPosX();
	player_start_y = level_map.getStartingPosY();
	player_map_x = player_start_x;
	player_map_y = player_start_y;
	player_screen_x = player_map_x + adjust_x;
	player_screen_y = player_map_y + adjust_y;
	vx = 0;
	vy = 0;
	player_d = 'n';

	player_size_x = all_textures[0].getSize().x / 2;
	player_size_y = all_textures[0].getSize().y / 2;

	sf::RectangleShape player;
	player.setPosition(player_screen_x - player_size_x, player_screen_y - player_size_y);
//	player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
	player.setTexture(&all_textures[0], true);

	sf::RectangleShape goal;
	goal.setSize(sf::Vector2f(54, 39));
	goal.setPosition(level_map.getGoalX() + adjust_x - 27, level_map.getGoalY() + adjust_y - 38);
	goal.setTexture(&all_textures[64]);

	vector<sf::RectangleShape> guards;
	sf::RectangleShape guard_template;
	guard_template.setSize(sf::Vector2f(all_textures[48].getSize().x, all_textures[48].getSize().y));
//	guard_template.setFillColor(sf::Color::Magenta);
	for (i = 0; i < level_map.getNumberOfGuards(); i++)
		guards.push_back(guard_template);

	viewport.setSize(window_width, window_height);
	viewport.setCenter(player_screen_x, player_screen_y);
	window.setView(viewport);

	sf::Text text;
	sf::Font font;
	font.loadFromFile(font_path);
	text.setFont(font);
	text.setColor(sf::Color::Red);
/*
	sf::Sound sound;
	sf::SoundBuffer soundBuffer;
	soundBuffer.loadFromFile("res/Sound.ogg");

	sound.setBuffer(soundBuffer);

	sound.play();

	sf::Music music;
	music.openFromFile("res/Music.ogg");
	music.play();
*/

	int anim = 0;
	float at = 0;
	fps = 0;
	timeval start;
	timeval current;
	gettimeofday(&start, NULL);
	while (window.isOpen())
	{
		// Fecha a janela caso solicitado
		sf::Event event;
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window.close();

		// Calcula o numero de frames por segundo
		gettimeofday(&current, NULL);
		if (current.tv_sec > start.tv_sec)
		{
			string s;
			while (fps > 0)
			{
				s = (char)((fps % 10) + '0') + s;
				fps /= 10;
			}
			text.setString(s + " fps");
			gettimeofday(&start, NULL);
		}
		fps++;

		// Limita o numero de frames por segundo
		while ((current.tv_sec - start.tv_sec) * 1000000.0 + current.tv_usec < fps * 1000000.0 / maxFps)
			gettimeofday(&current, NULL);

		// Controle de gravidade
		if (player_d == 'n')
			at += ati;
		else
			at = 0;

		// Controle de movimento
		char player_old_d = player_d;
		if (player_d == 'n')
		{
			// Caso o personagem esteja no ar, prossegue com o movimento em parabola
			int player_pos_x = player_start_x + vx * at;
			int player_pos_y = player_start_y + vy * at + k1 * at * at;
			int x = player_pos_x - player_map_x;
			int y = player_pos_y - player_map_y;
			Step step = level_map.step(player_map_x, player_map_y, player_pos_x, player_pos_y);
			player_map_x = step.x;
			player_map_y = step.y;
			player_d = step.d;
			if (player_d != 'n')
			{
				if (player_d == 'd')
				{
					player.setSize(sf::Vector2f(all_textures[33].getSize().x, all_textures[33].getSize().y));
					player_size_x = all_textures[33].getSize().x / 2;
					player_size_y = all_textures[33].getSize().y / 2 - 1;
					player.setTexture(&all_textures[player_facing_left ? 133 : 33], true);
					landing = true;
					anim = 0;
				}
				else
				{
					player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
					player_size_x = all_textures[0].getSize().x / 2;
					player_size_y = all_textures[0].getSize().y / 2;
					if (player_d == 'l')
						player.setTexture(&all_textures[118], true);
					else if (player_d == 'r')
						player.setTexture(&all_textures[18], true);
					else if (player_d == 'u' && player_facing_left)
						player.setTexture(&all_textures[110], true);
					else if (player_d == 'u' && !player_facing_left)
						player.setTexture(&all_textures[10], true);
				}
			}
			else
			{
				if (y > 0 && abs(y) > 4 * abs(x))
					player.setTexture(&all_textures[(player_facing_left ? 132 : 32)], true);
			}
		}
		else if ((player_d == 'l' || player_d == 'r') && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			// Controle de tecla pressionada
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				trigger_manager = true;

			// Caso o personagem esteja subindo uma parede a esquerda e o jogador pressiona a tecla D, o personagem pula da parede
			if (player_d == 'l' && sf::Keyboard::isKeyPressed(sf::Keyboard::D) && trigger_manager)
			{
				vx = take_off_speed;
				vy = 0;
				player_facing_left = false;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
				player.setSize(sf::Vector2f(all_textures[32].getSize().x, all_textures[32].getSize().y));
				player_size_x = all_textures[32].getSize().x / 2;
				player_size_y = all_textures[32].getSize().y / 2 - 1;
				player.setTexture(&all_textures[32], true);
				player_d = 'n';
			}
			// Mesma condicao acima, mas para paredes a direita do personagem
			else if (player_d == 'r' && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && trigger_manager)
			{
				vx = -take_off_speed;
				vy = 0;
				player_facing_left = true;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
				player.setSize(sf::Vector2f(all_textures[32].getSize().x, all_textures[0].getSize().y));
				player_size_x = all_textures[32].getSize().x / 2;
				player_size_y = all_textures[32].getSize().y / 2 - 1;
				player.setTexture(&all_textures[132], true);
				player_d = 'n';
			}
			else
			{
				// Se o jogador NAO estiver pressionando duas teclas direcionais ao mesmo tempo
				if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)))
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
						vy = -max_wall_speed;
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
						vy = max_wall_speed;
					// Se o jogador nao estiver pressionando nenhuma tecla
					else
						vy = 0;
				}
				// Se o jogador estiver pressionando duas teclas direcionais ao mesmo tempo, cancelar movimento
				else
					vy = 0;
				// Anda (ou muda de superficie) se o passo for possivel
				if (level_map.passable(player_map_x, player_map_y + vy, false))
				{
					int a;
					if (player_d == 'l')
						a = -2;
					else
						a = 2;
					// Se a parede continua ali, a funcao de passabilidade para (x + a, y) retorna false
					if (!level_map.passable(player_map_x + a, player_map_y + vy, false))
						player_map_y += vy;
					// Se nao ha mais parede, o personagem chegou num ponto onde ele pode mudar de superficie
					else
					{
						// Primeiro atualiza as novas coordenadas do personagem
						player_map_x += 2 * a;
						player_map_y += 2 * vy;
						level_map.updatePosA(player_map_x, player_map_y);

						// Em seguida procura a primeira area em que o personagem esta inserido
						for (i = 0; i < level_map.getNumberOfAreas(); i++)
							if (level_map.getPosA(i))
								break;
						area = level_map.getArea(i);

						// Corrige a posicao do personagem de acordo com os limites da area
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
						{
							player_map_y = area.bottom - 1;
							player_d = 'd';
							player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
							player_size_x = all_textures[0].getSize().x / 2;
							player_size_y = all_textures[0].getSize().y / 2;
							player.setTexture(&all_textures[player_facing_left ? 100 : 0], true);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && trigger_manager)
						{
							player_map_y = area.top + 1;
							player_d = 'u';
							player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
							player_size_x = all_textures[0].getSize().x / 2;
							player_size_y = all_textures[0].getSize().y / 2;
							player.setTexture(&all_textures[player_facing_left ? 110 : 10], true);

							// isso impede que o personagem pule da parede imediatamente depois de mudar de superficie
							trigger_manager = false;
						}
						// Prevencao de erro para o personagem nao se deslocar sozinho
						else
						{
							player_map_x -= 2 * a;
							player_map_y -= 2 * vy;
							level_map.updatePosA(player_map_x, player_map_y);
						}
					}
				}
				// Se o passo nao for possivel (atingiu outra superficie), muda de superficie
				else
				{
					for (i = 0; i < level_map.getNumberOfAreas(); i++)
						if (level_map.getPosA(i))
							break;
					area = level_map.getArea(i);
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
					{
						player_map_y = area.top + 1;
						player_d = 'u';
						player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
						player_size_x = all_textures[0].getSize().x / 2;
						player_size_y = all_textures[0].getSize().y / 2;
						player.setTexture(&all_textures[player_facing_left ? 110 : 10], true);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					{
						player_map_y = area.bottom - 1;
						player_d = 'd';
						player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
						player_size_x = all_textures[0].getSize().x / 2;
						player_size_y = all_textures[0].getSize().y / 2;
						player.setTexture(&all_textures[player_facing_left ? 100 : 0], true);
					}
				}
			}
		}
		else if ((player_d == 'u' || player_d == 'd') && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D) && landing)
			{
				if (++anim >= 6 * anim_frame_wait)
				{
					landing = false;
					anim = 0;
				}
				player.setTexture(&all_textures[(player_facing_left ? 133 : 33) + min(anim / anim_frame_wait, 2)], true);
			}
			else
			{
				player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
				player_size_x = all_textures[0].getSize().x / 2;
				player_size_y = all_textures[0].getSize().y / 2;
				landing = false;
			}
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				trigger_manager = true;
			if (player_d == 'u' && sf::Keyboard::isKeyPressed(sf::Keyboard::S) && trigger_manager)
			{
				vx = 0;
				vy = take_off_speed;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
				player_d = 'n';
				player.setSize(sf::Vector2f(all_textures[32].getSize().x, all_textures[32].getSize().y));
				player_size_x = all_textures[32].getSize().x / 2;
				player_size_y = all_textures[32].getSize().y / 2 - 1;
				player.setTexture(&all_textures[player_facing_left ? 132 : 32], true);
			}
			else
			{
				if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					{
						player_facing_left = true;
						if (player_d == 'd')
							vx = -max_ground_speed;
						else
							vx = -max_wall_speed;
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					{
						player_facing_left = false;
						if (player_d == 'd')
							vx = max_ground_speed;
						else
							vx = max_wall_speed;
					}
					else
						vx = 0;
				}
				else
					vx = 0;
				if (level_map.passable(player_map_x + vx, player_map_y, false))
				{
					run_into_wall = false;
					int a;
					if (player_d == 'u')
						a = -2;
					else
						a = 2;
					if (!level_map.passable(player_map_x + vx, player_map_y + a, false))
						player_map_x += vx;
					else if (player_d == 'd')
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
						{
							player_start_x = player_map_x;
							player_start_y = player_map_y;
							player_facing_left = true;
							vx = -take_off_speed;
							vy = 0;
							player_d = 'n';
							player.setSize(sf::Vector2f(all_textures[32].getSize().x, all_textures[32].getSize().y));
							player_size_x = all_textures[32].getSize().x / 2;
							player_size_y = all_textures[32].getSize().y / 2 - 1;
							player.setTexture(&all_textures[132], true);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
						{
							player_start_x = player_map_x;
							player_start_y = player_map_y;
							player_facing_left = false;
							vx = take_off_speed;
							vy = 0;
							player_d = 'n';
							player.setSize(sf::Vector2f(all_textures[32].getSize().x, all_textures[32].getSize().y));
							player_size_x = all_textures[32].getSize().x / 2;
							player_size_y = all_textures[32].getSize().y / 2 - 1;
							player.setTexture(&all_textures[32], true);
						}
					}
					else
					{
						player_map_x += 2 * vx;
						player_map_y += 2 * a;
						level_map.updatePosA(player_map_x, player_map_y);
						for (i = 0; i < level_map.getNumberOfAreas(); i++)
							if (level_map.getPosA(i))
								break;
						area = level_map.getArea(i);
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
						{
							player_map_x = area.right - 1;
							player_d = 'r';
							player.setTexture(&all_textures[9], true);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
						{
							player_map_x = area.left + 1;
							player_d = 'l';
							player.setTexture(&all_textures[109], true);
						}
						else
						{
							player_map_x -= 2 * vx;
							player_map_y -= 2 * a;
							level_map.updatePosA(player_map_x, player_map_y);
						}
						trigger_manager = false;
					}
				}
				else
				{
					run_into_wall = true;
					if (player_d == 'u')
					{
						for (i = 0; i < level_map.getNumberOfAreas(); i++)
							if (level_map.getPosA(i))
								break;
						area = level_map.getArea(i);
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
						{
							player_map_x = area.left + 1;
							player_d = 'l';
							player.setTexture(&all_textures[9], true);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
						{
							player_map_x = area.right - 1;
							player_d = 'r';
							player.setTexture(&all_textures[109], true);
						}
					}
				}
			}
		}
		if (player_old_d != player_d && player_d != 'd' && level_map.bumpAll(player_map_x, player_map_y))
		{
			vx = 0;
			vy = 0;
			at = 0;
			if (player_d == 'l')
				player_map_x++;
			else if (player_d == 'r')
				player_map_x--;
			if (player_d == 'u')
				player_map_y++;
			player_start_x = player_map_x;
			player_start_y = player_map_y;
			player_d = 'n';
		}

		// Conquista o objetivo da fase (dadas as condicoes)
		if (abs(player_map_x - level_map.getGoalX()) < button_dist_max && player_map_y == level_map.getGoalY() && !level_map.done())
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				player_map_x = level_map.getGoalX();
				level_map.finish();
				player_d = 'a';
				anim = 0;
				player.setSize(sf::Vector2f(all_textures[65].getSize().x, all_textures[65].getSize().y));
				player_size_x = all_textures[65].getSize().x / 2;
				player_size_y = all_textures[65].getSize().y / 2;
			}
		}

		// Procura botoes que podem ser pressionados
		// TODO

		// Atualiza a coordenada do personagem na tela e o viewport
		level_map.updatePosA(player_map_x, player_map_y);
		player_fake_pos_x = player_map_x;
		player_fake_pos_y = player_map_y;
		if (player_d != 'n')
		{
			for (i = 0; i < level_map.getNumberOfAreas(); i++)
				if (level_map.getPosA(i))
					break;
			area = level_map.getArea(i);
			if (!level_map.passable(player_map_x - player_size_x, player_map_y, false))
				player_fake_pos_x = area.left + player_size_x + 1;
			else if (!level_map.passable(player_map_x + player_size_x, player_map_y, false))
				player_fake_pos_x = area.right - player_size_x - 1;
			if (!level_map.passable(player_map_x, player_map_y - player_size_y, false))
				player_fake_pos_y = area.top + player_size_y + 1;
			else if (!level_map.passable(player_map_x, player_map_y + player_size_y, false))
				player_fake_pos_y = area.bottom - player_size_y - 1;
		}
		player_screen_x = player_fake_pos_x + adjust_x;
		player_screen_y = player_fake_pos_y + adjust_y;
		player.setPosition(player_screen_x - player_size_x, player_screen_y - player_size_y);
		player_screen_x = player_map_x + adjust_x;
		player_screen_y = player_map_y + adjust_y;
		viewport.setCenter(player_screen_x, player_screen_y);
		window.setView(viewport);

		// Atualiza o frame de animacao do personagem
		if (player_d == 'a')
		{
			if (++anim >= 34 * anim_frame_wait)
			{
				goal.setTexture(&all_textures[99]);
				player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
				player_size_x = all_textures[0].getSize().x / 2;
				player_size_y = all_textures[0].getSize().y / 2 - 1;
				player_facing_left = false;
				player_d = 'd';
				anim = 0;
			}
			else
				player.setTexture(&all_textures[65 + anim / anim_frame_wait], true);
		}
		else if (!landing && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (player_d == 'u' || player_d == 'd'))
			{
				if (player_d == 'd' && run_into_wall)
				{
					anim = 0;
					player.setTexture(&all_textures[player_facing_left ? 100 : 0], true);
				}
				else
				{
					if (++anim >= 8 * anim_frame_wait)
						anim = 0;
					player_facing_left = true;
					player.setTexture(&all_textures[((player_d == 'u') ? 110 : 101) + (anim / anim_frame_wait) % 8], true);
				}
			}
			else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (player_d == 'u' || player_d == 'd'))
			{
				if (player_d == 'd' && run_into_wall)
				{
					anim = 0;
					player.setTexture(&all_textures[player_facing_left ? 100 : 0], true);
				}
				else
				{
					if (++anim >= 8 * anim_frame_wait)
						anim = 0;
					player_facing_left = false;
					player.setTexture(&all_textures[((player_d == 'u') ? 10 : 1) + (anim / anim_frame_wait) % 8], true);
				}
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (player_d == 'l' || player_d == 'r'))
			{
				if (++anim >= 8 * anim_frame_wait)
					anim = 0;
				player.setTexture(&all_textures[((player_d == 'l') ? 119 : 19) + (anim / anim_frame_wait) % 8], true);
			}
			else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (player_d == 'l' || player_d == 'r'))
			{
				anim = 0;
				player.setTexture(&all_textures[((player_d == 'l') ? 109 : 9)], true);
			}
			else if (player_d == 'd')
				player.setTexture(&all_textures[(player_facing_left ? 100 : 0)], true);
		}
		else if (player_d == 'd' && jump_strength > 0)
				player.setTexture(&all_textures[(player_facing_left ? 127 : 27)], true);

		// Move e anima todos os guardas
		dead = level_map.guardsMove(player_map_x, player_map_y, vy, dead);
		for (i = 0; i < level_map.getNumberOfGuards(); i++)
			guards[i].setTexture(&all_textures[level_map.guardAnim(i)]);

		// Alinha o texto de acordo com a posicao do viewport
		text.setPosition(viewport.getCenter().x - window_width / 2, viewport.getCenter().y - window_height / 2);

		// Redesenha o conteudo da tela (mas nao atualiza)
		window.clear();
		window.draw(shape);
		if (player_d != 'a')
			window.draw(goal);
		window.draw(player);
		window.draw(text);
		for (i = 0; i < guards.size(); i++)
		{
			int gx = level_map.getGuardX(i) + adjust_x - all_textures[48].getSize().x / 2;
			int gy = level_map.getGuardY(i) + adjust_y - all_textures[48].getSize().y + 2;
			guards[i].setPosition(gx, gy);
			window.draw(guards[i]);
		}

		// Comeca o processo de calcular o salto caso caso o botao esquerdo do mouse esteja pressionado e o personagem nao esteja no ar
		if (player_d != 'n' && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			// Correcao de problema com animacao de queda
			landing = false;

			// Ajusta o tamanho do retangulo do personagem para desenhar a textura
			if (player_d == 'd')
			{
				player.setSize(sf::Vector2f(all_textures[27].getSize().x, all_textures[27].getSize().y));
				player_size_x = all_textures[27].getSize().x / 2;
				player_size_y = all_textures[27].getSize().y / 2 - 1;
			}

			// O salto ja comeca com uma forca inicial
			if (jump_strength == 0)
				jump_strength = jump_strength_start;

			// O salto cresce apenas ate um limite determinado arbitrariamente
			if (jump_strength < jump_strength_max)
				jump_strength = min(jump_strength + ji, jump_strength_max);

			// Sao realizados ajustes iniciais antes de desenhar a parabola com base na sua tangente
			sf::Vector2i p = sf::Mouse::getPosition() - window.getPosition();
			x = p.x - player_screen_x - (window_width / 2 - viewport.getCenter().x);
			float y0 = p.y - player_screen_y - (window_height / 2 - viewport.getCenter().y);
			float h = sqrt(x * x + y0 * y0);
			jump_strength_calc = jump_strength;
			if (h != 0) // Impede divisao por 0
			{
				// Prossegue com os calculos
				if (h < k2)
					jump_strength_calc = max((int) (jump_strength * h / k2), jump_strength_start);
				float xt, yt, t, k4 = k3 * jump_strength_calc / jump_strength_max;
				x /= (h / k2);
				y0 /= (h / k2);
				t = 1.0 / (jump_strength_calc * k0 / k2);
				y = jump_strength_calc * k0 * t * y0 / k2 - k0 * k4 * k1 * t * t; // obs: nao sei o que k3 faz na equacao mas funciona
				h = sqrt(x * x + y * y);

				// So desenha a parabola caso ela nao va logo de cara contra uma parede
				if (!(player_d == 'l' && x < 0 || player_d == 'r' && x > 0 || player_d == 'u' && y < 0 || player_d == 'd' && y > 0))
				{
					t = 0;
					sf::RectangleShape r;
					r.setSize(sf::Vector2f(5, 5));
					r.setFillColor(sf::Color::Red);
					vector<bool> old_pos_a = level_map.getPosA();
					xt = player_fake_pos_x;
					yt = player_fake_pos_y;
					int xt_prev, yt_prev;
					while (true)
					{
						// Impede divisao por 0
						if (h == 0)
							break;

						for (i = 0; i < itr; i++)
						{
							t += ati;

							// Guarda a posicao do ultimo ponto da parabola
							xt_prev = xt;
							yt_prev = yt;

							// Calcula a posicao do proximo ponto da parabola
							xt = player_fake_pos_x + jump_strength_calc * k0 * t * x / h;
							yt = player_fake_pos_y + jump_strength_calc * k0 * t * y / h + k1 * t * t;

							// Atualiza a animacao
							player_facing_left = (xt - xt_prev < 0);

							// Para de desenhar caso o trajeto seja impossivel
							if (!level_map.passable(xt, yt, false) && !level_map.bumpGlass(xt_prev, yt_prev, xt, yt, false))
								break;

							// Atualiza a posicao para continuar o calculo do trajeto
							level_map.updatePosA(xt, yt);
						}
						if (i < itr)
							break;

						// Desenha o ponto do trajeto
						r.setPosition(xt + adjust_x, yt + adjust_y);
						window.draw(r);
					}
					level_map.setPosA(old_pos_a);
				}
			}
		}
		// Executa o salto quando o botao esquerdo estiver solto
		else if (jump_strength > 0)
		{
			float h = sqrt(x * x + y * y);
			player_start_x = player_fake_pos_x;
			player_start_y = player_fake_pos_y;
			vx = jump_strength_calc * k0 * x / h;
			vy = jump_strength_calc * k0 * y / h;
			if (vx > 0)
				player_facing_left = false;
			else if (vx < 0)
				player_facing_left = true;
			if (player_d == 'l' && vx < 0)
			{
				vx = 0;
				vy = 0;
			}
			else if (player_d == 'r' && vx > 0)
			{
				vx = 0;
				vy = 0;
			}
			else if (player_d == 'u' && vy < 0)
			{
				vx = 0;
				vy = 0;
			}
			else if (player_d == 'd' && vy > 0)
			{
				vx = 0;
				vy = 0;
			}
			else
			{
				player_d = 'n';
				player.setSize(sf::Vector2f(all_textures[27].getSize().x, all_textures[27].getSize().y));
				player_size_x = all_textures[27].getSize().x / 2;
				player_size_y = all_textures[27].getSize().y / 2 - 1;
			}
			if (vx == 0 && vy == 0)
			{
				player.setSize(sf::Vector2f(all_textures[0].getSize().x, all_textures[0].getSize().y));
				player_size_x = all_textures[0].getSize().x / 2;
				player_size_y = all_textures[0].getSize().y / 2;
			}
			else if (abs(x) > 3 * abs(y))
				player.setTexture(&all_textures[(player_facing_left ? 130 : 30)], true);
			else if (abs(y) > 3 * abs(x) && y < 0)
				player.setTexture(&all_textures[(player_facing_left ? 128 : 28)], true);
			else if (y > 0)
				player.setTexture(&all_textures[(player_facing_left ? 131 : 31)], true);
			else
				player.setTexture(&all_textures[(player_facing_left ? 129 : 29)], true);
			jump_strength = 0;
		}

		// Atualiza a tela com o que foi desenhado na ultima iteracao
		window.display();
	}
	return 0;
}

