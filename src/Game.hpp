#ifndef GAME_HPP
#define GAME_HPP
#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "Tile.hpp"
#define WindowWidth 800
#define WindowHeight 600

class Game
{
public:
	Game();
	void run();
private:
	void meta_tiles_listen_event();
	void generateTiles();
	void processEvents();
	void update(sf::Time dt);
	void clear();
	void render();
	void find();
	std::vector< std::vector<int> > around(Tile &tile);
	void trace();
	void set_state(Tile &tile, std::string state);
	void handleMouseEvent(sf::Mouse::Button button, bool IsPressed);
	sf::Texture loadTexture(std::string file_path);

private:
	sf::RenderWindow mWindow;
	sf::Texture og_tile;
	std::vector< std::vector<Tile> > tiles;
	bool mouse_pressed, running_find, tracing;
	std::string mouse_button;
	std::string working_dir;
	float mWidth, mHeight;
	unsigned int width, height;
	unsigned int mouseX, mouseY;
	
	std::vector< std::vector<int> > openList;
	std::vector< std::vector<int> > closedList;
	std::vector< std::vector<int> > traversedList;
	Tile* begin, * end;
};
#endif

