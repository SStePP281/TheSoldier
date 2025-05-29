#pragma once
#ifndef MAPM
#define MAPM

#include "Map.h"
#include "Leaf.h"
#include "Resources.h"
#include "SoundManager.h"
#include "DataBase.h"
#include "EventSystem.h"
#include "CONST.h"
#include <SFML/Graphics/RenderWindow.hpp>

class MapManager
{
public:
	MapManager(sf::RenderWindow* _window);
	~MapManager();

	void save();
	void load(std::string fileName = "");

	sf::Vector2f nextLocation(int index);
	void rewriteSprites(std::vector<std::shared_ptr<Sprite>>* sprs);
	sf::Vector2f getStartPosition();
	void drawMap(int layer);
	Map* getNowMap();
private:
	Map* nowMap;
	sf::RenderWindow* window;
	sf::Vector2f startPos;
	sf::Vector2f endPos;

	void generate();
	void writeRoom(const sf::IntRect& rect, int layer, int value);
	void writeEnemy(const std::vector<sf::IntRect>& rooms);
};

#endif // !MAPM