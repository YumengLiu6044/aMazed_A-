#include "Tile.hpp"


Tile::Tile() {
	state = "empty";
	begin = false, end = false;
	F = 0;
	H = 0;
	G = 0;
}

void Tile::set_state(std::string state) {
	this->state = state;
}

void Tile::set_begin() {
	begin = true;
}

void Tile::set_end() {
	end = true;
}

void Tile::set_next() {
	state = "next";
}

void Tile::set_previous(Tile* tile) {
	previous = tile;
}

Tile* Tile::get_previous() {
	return previous;
}

bool Tile::get_begin() {
	return begin;
}