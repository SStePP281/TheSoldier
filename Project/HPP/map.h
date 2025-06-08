#ifndef MAP_H
#define MAP_H

#include "resources.h"
#include "sprite.h"
#include "const.h"
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

class Map {
public:
	void SetNewOnGrid(int x, int y, int layer_number, int value);
	const int GetOnGrid(int x, int y, int layer_number) const;
	const bool IsCellEmpty(const sf::Vector2i& position) const;
	void SetupBlockmap(Sprite* sprite);
	void DeleteInBlockMap(Sprite* sprite);
	const std::set<Sprite*>& GetBlockMap(const sf::Vector2i& position) const;
	void RotateSprite(const sf::Vector2i& position, float angle);
	void SetMapSprite(const MapSprite& map_sprite);
	void DeleteMapSprite(const sf::Vector2i& position);
	const std::vector<MapSprite>& GetMapSprites() const;

	std::vector<std::vector<std::array<int, kLayerCount>>> grid;
	std::vector<std::vector<std::set<Sprite*>>> block_map;
	std::vector<MapSprite> sprites;
private:
	const bool IsValidGridPos(int x, int y) const;
	const bool IsValidBlockmapPos(int x, int y) const;
	const bool InsertInBlockMap(sf::Vector2i position, Sprite* sprite);
	void RemoveInBlockMap(sf::Vector2i position, Sprite* sprite);
};

#endif // !MAP_H
