#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <sys/time.h>
#include "Map.h"

using namespace std;

// Informacoes basicas
string settings     = "settings.txt";
string map_name     = "map4.png";
string window_title = "Gunpoint Prototype";
string font_path    = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf";

// Controle de resolucao e fps
int window_width;
int window_height;
float zoom;
int fps;
int maxFps = 120;

// Controle de movimento e animacao do personagem
float x;
float y;
float vx;
float vy;
int max_ground_speed = 5;
int max_wall_speed = 2;
int take_off_speed = 30;
int jump_strength;
int jump_strength_start = 200;
int jump_strength_max = 500;
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

// Controle de teclas pressionadas
bool trigger_manager = true;

// Controle das areas nas quais o personagem esta inserido
vector<bool> player_a;
Area area;

// Dados relevantes do mapa da fase
Map level_map(map_name);

// Constantes
float ati = 0.04;	// valor somado a vy em cada iteracao
int itr = 5;
float ti = itr * ati;	// valor somado a t em cada iteracao
int ji = 5;      	// valor somado a jump_strength em cada iteracao
float k0 = 0.7;  	// constante multiplicadora de jump_strength
float k1 = 90.0; 	// aceleracao gravitacional do jogo
float k2 = 50.0; 	// modulo do vetor direcao
float k3 = 19.0; 	// magica. Manter valor entre 10 e 20

// Contador (pra nao precisar inicializar o mesmo contador varias vezes)
int i;

int main(void)
{
	window_width = 800;
	window_height = 600;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), window_title, sf::Style::Close);
	sf::View viewport;

	sf::Texture texture;
	texture.loadFromFile(map_name);

	zoom = 1.0;

	int adjust_x = (window_width - zoom * texture.getSize().x) / 2;
	int adjust_y = window_height - zoom * texture.getSize().y;

	sf::RectangleShape shape;
	shape.setPosition(adjust_x, adjust_y);
	shape.setSize(sf::Vector2f(zoom * texture.getSize().x, zoom * texture.getSize().y));
	shape.setTexture(&texture);

	player_start_x = level_map.getStartingPosX();
	player_start_y = level_map.getStartingPosY();
	player_map_x = player_start_x;
	player_map_y = player_start_y;
	player_screen_x = player_map_x * zoom + adjust_x;
	player_screen_y = player_map_y * zoom + adjust_y;
	vx = 0;
	vy = 0;
	player_d = 'n';

	sf::RectangleShape player;
	player.setPosition(player_screen_x - player_size_x, player_screen_y - player_size_y);
	player.setSize(sf::Vector2f(player_size_x * 2 + 1, player_size_y * 2 + 1));
	player.setFillColor(sf::Color::Blue);

	sf::RectangleShape player_real;
	player_real.setSize(sf::Vector2f(50, 50));
	player_real.setFillColor(sf::Color::Red);

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
			Step step = level_map.step(player_map_x, player_map_y, player_pos_x, player_pos_y);
			player_map_x = step.x;
			player_map_y = step.y;
			player_d = step.d;
		}
		else if (player_d == 'l' || player_d == 'r')
		{
			// Controle de tecla pressionada
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				trigger_manager = true;

			// Caso o personagem esteja subindo uma parede a esquerda e o jogador pressiona a tecla D, o personagem pula da parede
			if (player_d == 'l' && sf::Keyboard::isKeyPressed(sf::Keyboard::D) && trigger_manager)
			{
				vx = take_off_speed;
				vy = 0;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
				player_d = 'n';
			}
			// Mesma condicao acima, mas para paredes a direita do personagem
			else if (player_d == 'r' && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && trigger_manager)
			{
				vx = -take_off_speed;
				vy = 0;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
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
				if (level_map.passable(player_map_x, player_map_y + vy))
				{
					int a;
					if (player_d == 'l')
						a = -2;
					else
						a = 2;
					// Se a parede continua ali, a funcao de passabilidade para (x + a, y) retorna false
					if (!level_map.passable(player_map_x + a, player_map_y + vy))
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
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
						{
							player_map_y = area.top + 1;
							player_d = 'u';

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
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					{
						player_map_y = area.bottom - 1;
						player_d = 'd';
					}
				}
			}
		}
		else if (player_d == 'u' || player_d == 'd')
		{
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				trigger_manager = true;
			if (player_d == 'u' && sf::Keyboard::isKeyPressed(sf::Keyboard::S) && trigger_manager)
			{
				vx = 0;
				vy = take_off_speed;
				player_start_x = player_map_x;
				player_start_y = player_map_y;
				player_d = 'n';
			}
			else
			{
				if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					{
						if (player_d == 'd')
							vx = -max_ground_speed;
						else
							vx = -max_wall_speed;
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					{
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
				if (level_map.passable(player_map_x + vx, player_map_y))
				{
					int a;
					if (player_d == 'u')
						a = -2;
					else
						a = 2;
					if (!level_map.passable(player_map_x + vx, player_map_y + a))
						player_map_x += vx;
					else if (player_d == 'd')
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
						{
							player_start_x = player_map_x;
							player_start_y = player_map_y;
							vx = -take_off_speed;
							vy = 0;
							player_d = 'n';
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
						{
							player_start_x = player_map_x;
							player_start_y = player_map_y;
							vx = take_off_speed;
							vy = 0;
							player_d = 'n';
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
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
						{
							player_map_x = area.left + 1;
							player_d = 'l';
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
				else if (player_d == 'u')
				{
					for (i = 0; i < level_map.getNumberOfAreas(); i++)
						if (level_map.getPosA(i))
							break;
					area = level_map.getArea(i);
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					{
						player_map_x = area.left + 1;
						player_d = 'l';
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					{
						player_map_x = area.right - 1;
						player_d = 'r';
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
			if (!level_map.passable(player_map_x - player_size_x, player_map_y))
				player_fake_pos_x = area.left + player_size_x;
			else if (!level_map.passable(player_map_x + player_size_x, player_map_y))
				player_fake_pos_x = area.right - player_size_x;
			if (!level_map.passable(player_map_x, player_map_y - player_size_y))
				player_fake_pos_y = area.top + player_size_y;
			else if (!level_map.passable(player_map_x, player_map_y + player_size_y))
				player_fake_pos_y = area.bottom - player_size_y;
		}
		player_screen_x = player_fake_pos_x * zoom + adjust_x;
		player_screen_y = player_fake_pos_y * zoom + adjust_y;
		player.setPosition(player_screen_x - player_size_x, player_screen_y - player_size_y);
		player_screen_x = player_map_x * zoom + adjust_x;
		player_screen_y = player_map_y * zoom + adjust_y;
		player_real.setPosition(player_screen_x - 25, player_screen_y - 25);
		viewport.setCenter(player_screen_x, player_screen_y);
		window.setView(viewport);

		// Alinha o texto de acordo com a posicao do viewport
		text.setPosition(viewport.getCenter().x - window_width / 2, viewport.getCenter().y - window_height / 2);

		// Redesenha o conteudo da tela (mas nao atualiza)
		window.clear();
		window.draw(shape);
		window.draw(player_real);
		window.draw(player);
		window.draw(text);

		// Comeca o processo de calcular o salto caso caso o botao esquerdo do mouse esteja pressionado e o personagem nao esteja no ar
		if (player_d != 'n' && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
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
			if (h != 0) // Impede divisao por 0
			{
				// Prossegue com os calculos
				float xt, yt;
				x /= (h / k2);
				y0 /= (h / k2);
				float t = 1.0 / (jump_strength * k0 / k2);
				float k4 = k3 * jump_strength / jump_strength_max;
				y = jump_strength * k0 * t * y0 / k2 - k0 * k4 * k1 * t * t; // obs: nao sei o que k3 faz na equacao mas funciona
				h = sqrt(x * x + y * y);

				// Terminados os calculos iniciais, sao inicializadas as variaveis pra comecar o loop
				t = 0;
				sf::RectangleShape r;
				r.setSize(sf::Vector2f(5, 5));
				r.setFillColor(sf::Color::Red);
				vector<bool> old_pos_a = level_map.getPosA();
				while (true)
				{
					// Impede divisao por 0
					if (h == 0)
						break;

					for (i = 0; i < itr; i++)
					{
						// Calcula a posicao do proximo ponto da parabola
						xt = player_map_x + jump_strength * k0 * t * x / h;
						yt = player_map_y + jump_strength * k0 * t * y / h + k1 * t * t;

						// Para de desenhar caso o trajeto seja impossivel
						if (!level_map.passable(xt, yt))
							break;

						// Atualiza a posicao para continuar o calculo do trajeto
						level_map.updatePosA(xt, yt);

						t += ati;
					}
					if (i < itr)
						break;

					// Desenha o ponto do trajeto
					r.setPosition(xt * zoom + adjust_x, yt * zoom + adjust_y);
//					r.setPosition(xt - player_map_x + player_screen_x, yt - player_map_y + player_screen_y);
					window.draw(r);
				}
				level_map.setPosA(old_pos_a);
			}
		}
		// Executa o salto quando o botao esquerdo estiver solto
		else if (jump_strength > 0)
		{
//			at = 0;
			float h = sqrt(x * x + y * y);
			player_start_x = player_map_x;
			player_start_y = player_map_y;
//			level_map.updatePosA(player_map_x, player_map_y);
			vx = jump_strength * k0 * x / h;
			vy = jump_strength * k0 * y / h;
			player_d = 'n';
			jump_strength = 0;
		}

		// Atualiza a tela com o que foi desenhado na ultima iteracao
		window.display();
	}
	return 0;
}

