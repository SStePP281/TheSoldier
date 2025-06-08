#ifndef MAPM
#define MAPM

#include "map.h"
#include "leaf.h"
#include "resources.h"
#include "sound_manager.h"
#include "data_base.h"
#include "event_system.h"
#include "const.h"
#include <SFML/Graphics/RenderWindow.hpp>

class MapManager {
public:
	MapManager(sf::RenderWindow* window);
	~MapManager();
	void Save();
	void Load(std::string file_name = "");
	const sf::Vector2f& NextLocation(int index);
	void RewriteSprites(const std::vector<std::shared_ptr<Sprite>>* const sprites);
	const sf::Vector2f& GetStartPosition() const;
	void DrawMap(int layer);
	Map* const GetNowMap() const;
private:
	void Generate();
	void WriteRoom(const sf::IntRect& rect, int layer, int value);
	void WriteEnemy(const std::vector<sf::IntRect>& rooms);
	void WriteHall(sf::Vector2i start_position, bool is_vertical);

	Map* now_map_;
	sf::RenderWindow* window_;
	sf::Vector2f start_position_;
	sf::Vector2f end_position_;
};

#endif // !MAPM
