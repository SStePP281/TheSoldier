#include "map.h"

const bool Map::IsValidGridPos(int x, int y) const {
	return y >= 0 && y < grid.size() && x >= 0 && x < grid[y].size();
}

const bool Map::IsValidBlockmapPos(int x, int y) const {
	return y >= 0 && y < block_map.size() && x >= 0 && x < block_map[y].size();
}

void Map::SetNewOnGrid(int x, int y, int layer_number, int value) {
	if (IsValidGridPos(x, y)){ grid[y][x][layer_number] = value; }
}

const int Map::GetOnGrid(int x, int y, int layer_number) const {
	return IsValidGridPos(x, y) ? grid[y][x][layer_number] : 0;
}

const bool Map::IsCellEmpty(const sf::Vector2i& position) const {
	return IsValidBlockmapPos(position.x, position.y) ? block_map[position.y][position.x].empty() : false;
}

void Map::RotateSprite(const sf::Vector2i& position, float angle) {
	if (!IsValidBlockmapPos(position.x, position.y)) return;

	auto sprite_iter = std::find_if(sprites.begin(), sprites.end(), [position](MapSprite map_sprite) 
		{return (sf::Vector2i)map_sprite.position == position; });
	
	if (sprite_iter != sprites.end()) { sprite_iter->angle += angle; }
}

void Map::SetupBlockmap(Sprite* sprite) {
	sf::Vector2f half_size = { sprite->GetSize() / 2.f, sprite->GetSize() / 2.f };
	sf::Vector2i start = (sf::Vector2i)(sprite->GetPosition() - half_size);
	sf::Vector2i end = (sf::Vector2i)(sprite->GetPosition() + half_size);

	std::set<std::tuple<int, int>> coords;
	for (int y = start.y; y <= end.y; y++) {
		for (int x = start.x; x <= end.x; x++) { 
			coords.insert({ x, y }); 
		}
	}

	std::set<std::tuple<int, int>> to_remove;
	auto blockmap_coords = sprite->GetBlockmapCoords();
	std::set_difference(blockmap_coords.begin(), blockmap_coords.end(),
		coords.begin(), coords.end(),
		std::inserter(to_remove, to_remove.end()));

	std::set<std::tuple<int, int>> to_insert;
	std::set_difference(coords.begin(), coords.end(), blockmap_coords.begin(),
		blockmap_coords.end(),
		std::inserter(to_insert, to_insert.end()));

	for (const auto& [x, y] : to_remove) { RemoveInBlockMap({ x, y }, sprite); }
	for (const auto& [x, y] : to_insert) { InsertInBlockMap({ x, y }, sprite); }

	sprite->SetBlockmapCoords(coords);
}

void Map::DeleteInBlockMap(Sprite* sprite) {
	for (const auto& [x, y] : sprite->GetBlockmapCoords()) {
		block_map[y][x].erase(sprite);
	}
}

const std::set<Sprite*>& Map::GetBlockMap(const sf::Vector2i& position) const {
	std::set<Sprite*> base;
	return IsValidBlockmapPos(position.x, position.y) ? block_map[position.y][position.x] : base;
}

const std::vector<MapSprite>& Map::GetMapSprites() const  { return sprites; }

void Map::SetMapSprite(const MapSprite& map_sprite) {
	for (const auto& s : sprites) {
		if (s.position == map_sprite.position && s.sprite_def_id == map_sprite.sprite_def_id) return;
	}

	sprites.push_back(map_sprite);
}

void Map::DeleteMapSprite(const sf::Vector2i& position) {
	auto it = std::remove_if(sprites.begin(), sprites.end(), [position](MapSprite map_sprite) {
		return (sf::Vector2i)map_sprite.position == position; });

	if (it != sprites.end()) { sprites.erase(it); }
}

const bool Map::InsertInBlockMap(sf::Vector2i position, Sprite* sprite) {
	if (!IsValidBlockmapPos(position.x, position.y)) return false;
	block_map[position.y][position.x].insert(sprite);
	return true;
}

void Map::RemoveInBlockMap(sf::Vector2i position, Sprite* sprite) {
	if (IsValidBlockmapPos(position.x, position.y)) { block_map[position.y][position.x].erase(sprite); }
}
