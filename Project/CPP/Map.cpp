#include "Map.h"

bool Map::isValidGridPos(int x, int y)
{
	return y >= 0 && y < grid.size() && x >= 0 && x < grid[y].size();
}

bool Map::isValidBlockmapPos(int x, int y)
{
	return y >= 0 && y < blockMap.size() && x >= 0 && x < blockMap[y].size();
}

void Map::SetNewOnGrid(int x, int y, int layerNumber, int value)
{
	if (isValidGridPos(x, y)){ grid[y][x][layerNumber] = value; }
}

int Map::GetOnGrid(int x, int y, int layerNumber)
{
	return isValidGridPos(x, y) ? grid[y][x][layerNumber] : 0;
}

bool Map::isCellEmpty(const sf::Vector2i& pos)
{
	return isValidBlockmapPos(pos.x, pos.y) ? blockMap[pos.y][pos.x].empty() : false;
}

void Map::rotateSprite(const sf::Vector2i& pos, float angle)
{
	if (!isValidBlockmapPos(pos.x, pos.y)) return;

	auto spriteIt = std::find_if(sprites.begin(), sprites.end(), [pos](MapSprite sp) 
		{return (sf::Vector2i)sp.position == pos; });
	
	if (spriteIt != sprites.end()) { spriteIt->angle += angle; }
}

void Map::setupBlockmap(Sprite* sp)
{
	sf::Vector2f halfSize = { sp->spDef.size / 2.f, sp->spDef.size / 2.f };
	sf::Vector2i start = (sf::Vector2i)(sp->spMap.position - halfSize);
	sf::Vector2i end = (sf::Vector2i)(sp->spMap.position + halfSize);

	std::set<std::tuple<int, int>> coords;
	for (int y = start.y; y <= end.y; y++) {
		for (int x = start.x; x <= end.x; x++) { coords.insert({ x, y }); }
	}

	std::set<std::tuple<int, int>> to_remove;
	auto blockmap_coords = sp->blockmap_coords;
	std::set_difference(blockmap_coords.begin(), blockmap_coords.end(),
		coords.begin(), coords.end(),
		std::inserter(to_remove, to_remove.end()));

	std::set<std::tuple<int, int>> to_insert;
	std::set_difference(coords.begin(), coords.end(), blockmap_coords.begin(),
		blockmap_coords.end(),
		std::inserter(to_insert, to_insert.end()));

	for (const auto& [x, y] : to_remove) { removeInBlockMap({ x, y }, sp); }
	for (const auto& [x, y] : to_insert) { insertInBlockMap({ x, y }, sp); }

	sp->blockmap_coords = coords;
}

void Map::deleteInBlockMap(Sprite* sp)
{
	for (const auto& [x, y] : sp->blockmap_coords)
	{
		blockMap[y][x].erase(sp);
	}
}

std::set<Sprite*> Map::getBlockMap(const sf::Vector2i& pos) 
{
	return isValidBlockmapPos(pos.x, pos.y) ? blockMap[pos.y][pos.x] : std::set<Sprite*>{};
}

std::vector<MapSprite>& Map::getMapSprites() { return sprites; }

void Map::setMapSprite(const MapSprite& sp)
{
	for (auto s : sprites)
	{
		if (s.position == sp.position && s.spriteDefId == sp.spriteDefId) return;
	}
	sprites.push_back(sp);
}

void Map::deleteMapSprite(const sf::Vector2i& pos)
{
	auto it = std::remove_if(sprites.begin(), sprites.end(), [pos](MapSprite sp) {
		return (sf::Vector2i)sp.position == pos; });

	if (it != sprites.end()) { sprites.erase(it); }
}

bool Map::insertInBlockMap(sf::Vector2i pos, Sprite* sprite)
{
	if (!isValidBlockmapPos(pos.x, pos.y)) return false;
	blockMap[pos.y][pos.x].insert(sprite);
	return true;
}

void Map::removeInBlockMap(sf::Vector2i pos, Sprite* sprite)
{
	if (isValidBlockmapPos(pos.x, pos.y)) { blockMap[pos.y][pos.x].erase(sprite); }
}