#pragma once
#ifndef MAPM
#define MAPM

#include "Map.h"
#include "Leaf.h"
#include "Resources.h"
#include "SoundManager.h"
#include "DataBase.h"
#include "EventSystem.h"
#include <SFML/Graphics/RenderWindow.hpp>

constexpr int SPACE_SIZE_W = 40, SPACE_SIZE_H = 40;
constexpr int ENEMY_LEVEL_COUNT = 40;
constexpr int NEXT_LEVEL_N = -1, BASE_N = 0, ARENA_1_N = 1, ARENA_2_N = 2, ARENA_3_N = 3, BOSS_N = 4;

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
	void writeRoom(sf::IntRect rect, int layer, int value);
	void writeEnemy(std::vector<sf::IntRect> rooms);
};

static std::vector<std::string> mapFileNames{
	"base.map",
	"converter1.map",
	"converter2.map",
	"converter3.map",
	"boss.map" };

#endif // !MAPM