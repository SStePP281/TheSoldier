#pragma once
#ifndef MAP_H
#define MAP_H

#include "Resources.h"
#include "Sprite.h"
#include "CONST.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <fstream>
#include <array>
#include <ios>
#include <string>
#include <set>

class Map
{
public:
	void SetNewOnGrid(int x, int y, int layerNumber, int value);
	int GetOnGrid(int x, int y, int layerNumber);

	bool isCellEmpty(sf::Vector2i& pos);
	void setupBlockmap(Sprite* sp);
	void deleteInBlockMap(Sprite* sp);
	std::set<Sprite*> getBlockMap(sf::Vector2i& pos);

	void rotateSprite(sf::Vector2i& pos, float angle);

	void setMapSprite(MapSprite& sp);
	void deleteMapSprite(sf::Vector2i& pos);
	std::vector<MapSprite>& getMapSprites();

	std::vector<std::vector<std::array<int, LAYER_COUNT>>> grid;
	std::vector<std::vector<std::set<Sprite*>>> blockMap;

	std::vector<MapSprite> sprites;
private:
	bool isValidGridPos(int x, int y);
	bool isValidBlockmapPos(int x, int y);
	bool insertInBlockMap(sf::Vector2i pos, Sprite* sprite);
	void removeInBlockMap(sf::Vector2i pos, Sprite* sprite);
};

#endif // !MAP_H