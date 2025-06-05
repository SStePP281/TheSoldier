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
	MapManager(sf::RenderWindow* window);
	~MapManager();

	void Save();
	void Load(std::string file_name = "");

	sf::Vector2f NextLocation(int index);
	void RewriteSprites(std::vector<std::shared_ptr<Sprite>>* sprites);
	sf::Vector2f GetStartPosition();
	void DrawMap(int layer);
	Map* GetNowMap();
private:
	void Generate();
	void WriteRoom(const sf::IntRect& rect, int layer, int value);
	void WriteEnemy(const std::vector<sf::IntRect>& rooms);
	void WriteHall(sf::Vector2i start_position, bool is_vertical);

	Map* now_map;
	sf::RenderWindow* window;
	sf::Vector2f start_position;
	sf::Vector2f end_position;
};

#endif // !MAPM
