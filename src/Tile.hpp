#ifndef TILE_HPP
#define TILE_HPP
#include "SFML/Graphics.hpp"
class Tile : public sf::Sprite
{
public:
	Tile();
	void set_state(std::string state);
	void set_previous(Tile* tile);
	Tile* get_previous();
	void set_begin();
	bool get_begin();
	void set_end();
	void set_next();
	std::string state;
	int x_pos, y_pos;
	double F, H, G;

private:
	Tile* previous;
	bool begin, end;
};
#endif