#include "Game.hpp"
#include <__config>
#include <algorithm>
#include <cmath>
#include <vector>
Game::Game() :
	mWindow(sf::VideoMode(WindowWidth, WindowHeight), "Maze Solver"),
	og_tile()
{
	mouse_pressed = false;
	running_find = false;
	tracing = false;

	mouse_button = "none";
	working_dir = "src/resources/";

	mouseX = 0, mouseY = 0;

	mWindow.setFramerateLimit(30);
	std::string message;
	message = "Keys\tFunction\n";
	message += "C\tClear Screen\n";
	message += "SPACE\tStart Tracing\n";
	std::cout << message << std::endl;
	sf::Vector2u size = mWindow.getSize();
	og_tile = loadTexture("empty_tile.png");
	mWidth = (float)size.x;
	mHeight = (float)size.y;
	width = (unsigned int)(mWidth / 10.0f);
	height = (unsigned int)(mHeight / 10.0f);
	generateTiles();

	openList.push_back(std::vector<int>{begin->x_pos, begin->y_pos});
}

void Game::run()
{
	while (mWindow.isOpen())
	{
		if (!(running_find || tracing))
		{
			processEvents();
		}
		else if (running_find)
		{
			find();
		}
		else
		{
			trace();
		}
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	sf::Mouse mouse;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		
		case sf::Event::MouseButtonPressed:
			handleMouseEvent(event.mouseButton.button, true);
			break;
		case sf::Event::MouseButtonReleased:
			handleMouseEvent(event.mouseButton.button, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		
		default:
			break;

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			running_find = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			clear();
		}
		
	}
	mouseX = mouse.getPosition(mWindow).x;
	mouseY = mouse.getPosition(mWindow).y;
	if (mouse.getPosition(mWindow).x < 0)
	{
		mouseX = 0;
	}
	if (mouse.getPosition(mWindow).y < 0)
	{
		mouseY = 0;
	}

	if (mouseX > mWidth)
	{
		mouseX = mWidth - 1;
	}
	if (mouseY > mHeight)
	{
		mouseY = mHeight - 1;
	}
	meta_tiles_listen_event();
}
void Game::update(sf::Time dt)
{
}
void Game::clear()
{
	openList.clear();
	closedList.clear();
	traversedList.clear();
	for (unsigned int i = 0; i < width; i++)
	{
		for (unsigned int j = 0; j < height; j++)
		{

			Tile& thisTile = tiles[i][j];
			if (i == 0 || i == (width - 1))
			{
				set_state(thisTile, "wall");
			}
			else if (j == 0 || j == (height - 1))
			{
				set_state(thisTile, "wall");
			}
			else {
				set_state(thisTile, "empty");
			}
			
		}
		
	}
	set_state(tiles[4][56], "begin");
	set_state(tiles[76][4], "end");
	openList.push_back(std::vector<int>{begin->x_pos, begin->y_pos});
	begin = &tiles[4][56];
	end = &tiles[76][4];

}
void Game::render()
{
	for (auto i : openList) {
		tiles[i[0]][i[1]].setColor(sf::Color::Yellow);
	}
	for (auto i : closedList) {
		tiles[i[0]][i[1]].setColor(sf::Color::Green);
	}
	for (auto i : traversedList) {
		tiles[i[0]][i[1]].setColor(sf::Color::Blue);
	}

	mWindow.clear(sf::Color(255, 255, 255, 255));
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			mWindow.draw(tiles[i][j]);
		}
	}
	mWindow.display();
}

double getDistance(std::vector<int> from, std::vector<int> to) {

	return (from[0] - to[0]) * (from[0] - to[0]) + (from[1] - to[1]) * (from[1] - to[1]);
}

void Game::find()
{
	running_find = true;
	if (end->state != "next_tested")
	{
		if (openList.size() == 0) {
			clear();
			running_find = false;
		}
		std::vector<int> current = openList[0];
		for (auto i : openList) {
			if (tiles[i[0]][i[1]].F < tiles[current[0]][current[1]].F || 
				(tiles[i[0]][i[1]].F == tiles[current[0]][current[1]].F &&
				tiles[i[0]][i[1]].H < tiles[current[0]][current[1]].H)) {
					current = i;
			}
		}

		closedList.push_back(current);
		openList.erase(std::find(openList.begin(), openList.end(), current));


		for (auto neighborLoc : around(tiles[current[0]][current[1]])) {
			bool inSearch = std::find(openList.begin(), openList.end(), neighborLoc) != openList.end();
			double costToNeighbor = tiles[current[0]][current[1]].G + getDistance(current, neighborLoc);

			if (!inSearch || costToNeighbor < tiles[neighborLoc[0]][neighborLoc[1]].G) {
				tiles[neighborLoc[0]][neighborLoc[1]].G = costToNeighbor;
				tiles[neighborLoc[0]][neighborLoc[1]].set_previous(&tiles[current[0]][current[1]]);
				set_state(tiles[neighborLoc[0]][neighborLoc[1]], "next_tested");

				if (!inSearch) {
					tiles[neighborLoc[0]][neighborLoc[1]].H = getDistance(neighborLoc, std::vector<int>{end->x_pos, end->y_pos});
					openList.push_back(neighborLoc);
				}
			}

		}

	}
	else
	{
		running_find = false;
		tracing = true;
	}
}

std::vector< std::vector<int> > Game::around(Tile &tile)
{
	int x = tile.x_pos;
	int y = tile.y_pos;

	std::vector< std::vector<int> > locs = {
		{x + 1, y}, 
		{x , y + 1},
		{x - 1, y},
		{x, y - 1}
	};
	
	std::vector< std::vector<int> > result;

	for (auto i : locs) {
		int tileRow = i[0];
		int tileCol = i[1];

		if (tileRow >= this->tiles.size() || tileRow < 0) {
			continue;
		}
		if (tileCol >= this->tiles[0].size() || tileCol < 0) {
			continue;
		}

		if (this->tiles[tileRow][tileCol].state != "wall" &&
			(std::find(closedList.begin(), closedList.end(), i) == closedList.end())) {
			result.push_back(i);
		}
	}


	return result;

}

void Game::set_state(Tile& tile, std::string state)
{
	tile.set_state(state);
	if (state == "empty") tile.setColor(sf::Color::White);
	else if (state == "wall") tile.setColor(sf::Color::Black);
	else if (state == "begin")
	{
		tile.setColor(sf::Color::Blue);
		tile.set_begin();
		begin = &tile;
	}
	else if (state == "end")
	{
		tile.setColor(sf::Color::Red);
		tile.set_end();
		end = &tile;
	}
	else if (state == "tested") tile.setColor(sf::Color::Green);
	else if (state == "next_tested") tile.setColor(sf::Color::Yellow);
	else tile.setColor(sf::Color::Yellow);
}

void Game::handleMouseEvent(sf::Mouse::Button button, bool IsPressed)
{
	
	mouse_pressed = IsPressed;
	if (!(mouse_pressed))
	{
		mouse_button = "none";
	}
	else
	{
		if (button == sf::Mouse::Left)
		{
			mouse_button = "left";
		}
		else if (button == sf::Mouse::Right)
		{
			mouse_button = "right";
		}
	}
}

sf::Texture Game::loadTexture(std::string file_path)
{
	sf::Texture loaded_texture;
	if (!loaded_texture.loadFromFile(working_dir + file_path))
	{
		std::cout << "Can't load:\t" << file_path << std::endl;
	}
	return loaded_texture;
}

void Game::generateTiles()
{
	for (unsigned int i = 0; i < width; i++)
	{
		std::vector<Tile> these_tiles;
		for (unsigned int j = 0; j < height; j++)
		{

			Tile tile;
			tile.setTexture(og_tile);
			set_state(tile, "empty");
			tile.setPosition(i * 10, j * 10);
			tile.x_pos = i;
			tile.y_pos = j;
			if (i == 0 || i == (width - 1))
			{
				set_state(tile, "wall");
			}
			if (j == 0 || j == (height - 1))
			{
				set_state(tile, "wall");
			}
			these_tiles.push_back(tile);

		}
		tiles.push_back(these_tiles);
	}
	set_state(tiles[4][56], "begin");
	set_state(tiles[76][4], "end");
}
void Game::meta_tiles_listen_event()
{
	unsigned int this_width = mouseX / 10;
	unsigned int this_height = mouseY / 10;
	if (this_width >= (unsigned int)(mWidth / 10.0f))
	{
		this_width = mWidth / 10.0f - 1;
	}
	if (this_height >= (unsigned int)(mHeight / 10.0f))
	{
		this_height = mHeight / 10.0f - 1;
	}
	if (mouse_button == "left")
	{
		if (!(tiles[this_width][this_height].state == "begin" || tiles[this_width][this_height].state == "end" || tiles[this_width][this_height].state == "next"))
			set_state(tiles[this_width][this_height], "wall");
	}
	else if (mouse_button == "right")
	{
		if (!(tiles[this_width][this_height].state == "begin" || tiles[this_width][this_height].state == "end" || tiles[this_width][this_height].state == "next"))
		{
			if (!(this_width == 0 || this_height == 0 || this_width == (width - 1) || this_height == (height - 1)))
			{
				set_state(tiles[this_width][this_height], "empty");
			}
		}
	}
}

void Game::trace()
{
	traversedList.push_back(std::vector<int>{end->x_pos, end->y_pos});
	Tile* tempTile = end;
	end = end->get_previous();
	if (end == begin)
	{	
		clear();
		tracing = false;
	}
	tempTile->set_previous(nullptr);
}