#include "map_manager.h"

MapManager::MapManager(sf::RenderWindow* window) : 
	window_{ window }, now_map_{ nullptr } {
	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int NON) { Save(); });

	event.Subscribe<int>("RESET_GAME", [&](const int NON) {
		auto& state = GameState::GetInstance(); 
		state.data.is_level_base = true; 
		state.data.level_number = 0;
		event.Trigger<int>("SWAPLOC", kBaseNumber);
		});

	event.Subscribe<int>("WIN_GAME", [&](const int NON) {
		Load(map_file_names[kBaseNumber]);
		auto& state = GameState::GetInstance();
		state.data.is_level_base = true;
		state.data.level_number = 0;
		event.Trigger<int>("SWAPLOC", 0);
		});
}

MapManager::~MapManager() {
	delete now_map_;
}

void MapManager::Save() {
	std::ofstream out{ "Data/current.map", std::ios::out | std::ios::binary};
	if (!out.is_open()) return;
	if (now_map_->grid.empty()) return;

	auto grid = now_map_->grid;
	size_t h = grid.size();
	size_t w = grid[0].size();

	out.write(reinterpret_cast<char*>(&w), sizeof(w));
	out.write(reinterpret_cast<const char*>(&h), sizeof(h));

	for (size_t y = 0; y < h; y++) {
		for (size_t x = 0; x < w; x++) {
			out.write(reinterpret_cast<char*>(grid[y][x].data()),
				sizeof(grid[y][x][0]) * kLayerCount);
		}
	}

	auto sprites = now_map_->sprites;
	size_t sprite_count = sprites.size();
	out.write(reinterpret_cast<char*>(&sprite_count), sizeof(sprite_count));

	for (size_t i = 0; i < sprites.size(); i++) {
		out.write(reinterpret_cast<char*>(&sprites[i]), sizeof(sprites[i]));
	}

	out.flush();
	out.close();
}

void MapManager::Load(std::string file_name) {
	std::ifstream in;
	if (file_name == "") { 
		in = std::ifstream{ "Data/current.map", std::ios::in | std::ios::binary }; 
	} else {
		file_name = "Data/" + file_name;
		in = std::ifstream{ file_name, std::ios::in | std::ios::binary };
	}

	if (!in.is_open()) return;

	now_map_ = new Map();

	size_t h = 0, w = 0;
	in.read(reinterpret_cast<char*>(&w), sizeof(w));
	in.read(reinterpret_cast<char*>(&h), sizeof(h));

	if (h == 0 && w == 0) {
		h = kSpaceSizeHeight;
		w = kSpaceSizeWight;
	}

	now_map_->grid = std::vector(h, std::vector(w, std::array<int, kLayerCount>()));
	now_map_->block_map = std::vector(h, std::vector(w, std::set<Sprite*>()));

	for (size_t y = 0; y < h; y++) {
		for (size_t x = 0; x < w; x++) {
			in.read(reinterpret_cast<char*>(now_map_->grid[y][x].data()),
				sizeof(now_map_->grid[y][x][0]) * kLayerCount);
		}
	}

	size_t sprite_count;
	in.read(reinterpret_cast<char*>(&sprite_count), sizeof(sprite_count));

	now_map_->sprites = std::vector<MapSprite>(sprite_count);
	for (size_t i = 0; i < now_map_->sprites.size(); i++) {
		in.read(reinterpret_cast<char*>(&now_map_->sprites[i]), sizeof(now_map_->sprites[i]));
	}

	in.close();
}
 
const sf::Vector2f& MapManager::NextLocation(int index) {
	auto& state = GameState::GetInstance();

	if (index == kBaseNumber) {
		state.data.is_level_base = true;

		SoundManager::PlayerMusic(MusicType::BaseSound);

		Load(map_file_names[kBaseNumber]);
		end_position_ = {0.0f, 0.0f};

		for (const auto& sp : now_map_->sprites) {
			if (sp.sprite_def_id == 0) {
				start_position_ = sp.position;
				break;
			}
		}
	} else {
		state.data.is_level_base = false;

		SoundManager::PlayerMusic(MusicType::LevelSound);

		if (index == kNextLevelNumber) {
			state.data.level_number++;

			Generate();
		} else {
			Load(map_file_names[index]);

			for (const auto& sp : now_map_->sprites) {
				if (sp.sprite_def_id == 0) {
					start_position_ = sp.position;
					break;
				}
			}

		}
	}

	return start_position_;
}

void MapManager::RewriteSprites(const std::vector<std::shared_ptr<Sprite>>* const sprites) {
	now_map_->sprites.clear();
	for (const auto& sp : *sprites) { now_map_->sprites.push_back(sp->GetMapSprite()); }
}

const sf::Vector2f& MapManager::GetStartPosition() const { return start_position_; }

void MapManager::DrawMap(int layer_number) {
	if (now_map_->grid.empty()) return;

	sf::RectangleShape cell(sf::Vector2f(kTextureSize * 0.95f, kTextureSize * 0.95f));
	cell.setTexture(&Resources::textures);

	if (layer_number != kSpeiteLayer) {
		for (size_t y = 0; y < now_map_->grid.size(); y++) {
			for (size_t x = 0; x < now_map_->grid[y].size(); x++) {
				int value = now_map_->grid[y][x][layer_number];

				if (value != 0) {
					cell.setTextureRect(sf::IntRect(sf::Vector2i((value - 1) % kTextureCount * kTextureSize,
						(value - 1) / kTextureCount * kTextureSize),
						sf::Vector2i(kTextureSize, kTextureSize)));
					cell.setPosition((float)kTextureSize * (sf::Vector2f((float)x, (float)y) + sf::Vector2f(0.025f, 0.025f)));

					window_->draw(cell);
				}
			}
		}
	} else {
		cell.setFillColor(sf::Color(255, 255, 255, 100));

		for (size_t y = 0; y < now_map_->grid.size(); y++) {
			for (size_t x = 0; x < now_map_->grid[y].size(); x++) {
				int value = now_map_->grid[y][x][kWallLayer];

				if (value != 0) {
					cell.setTextureRect(sf::IntRect(sf::Vector2i((value - 1) % kTextureCount * kTextureSize,
						(value - 1) / kTextureCount * kTextureSize),
						sf::Vector2i(kTextureSize, kTextureSize)));
					cell.setPosition((float)kTextureSize * (sf::Vector2f((float)x, (float)y) + sf::Vector2f(0.025f, 0.025f)));

					window_->draw(cell);
				}
			}
		}

		sf::RectangleShape spite_shape(sf::Vector2f(kIconSize, kIconSize));
		spite_shape.setTexture(&Resources::sprite_icon);

		for (const auto& sp : now_map_->sprites) {
			if (sp.sprite_def_id != 0) {
				spite_shape.setTextureRect(sf::IntRect(sf::Vector2i(kIconSize * (sp.sprite_def_id - 1), 0),
					sf::Vector2i(kIconSize, kIconSize)));
				spite_shape.setOrigin(spite_shape.getLocalBounds().width / 2, spite_shape.getLocalBounds().height / 2);
				spite_shape.setPosition((float)kTextureSize * sp.position);
				spite_shape.rotate(sp.angle);

				window_->draw(spite_shape);
			}
		}
	}
}

void MapManager::Generate() {
	Leaf* root = new Leaf({ 0,0 }, { kSpaceSizeWight, kSpaceSizeHeight });
	std::vector<Leaf*> temp_leafs;
	temp_leafs.push_back(root);

	bool did_split = true;

	while (did_split) {
		did_split = false;

		for (int i = 0; i < temp_leafs.size(); i++) {
			if (!temp_leafs[i]->left_child && !temp_leafs[i]->right_child) {
				if (temp_leafs[i]->leaf_data.width > kMaxLeafSize || temp_leafs[i]->leaf_data.height > kMaxLeafSize || Random::BitRandom() > 0.25) {
					if (temp_leafs[i]->Split()) {
						temp_leafs.push_back(temp_leafs[i]->left_child);
						temp_leafs.push_back(temp_leafs[i]->right_child);

						did_split = true;
					}
				}
			}
		}
	}

	root->FindRoom();
	root->GetAllChild();

	std::vector<sf::IntRect> rooms;
	std::vector<std::pair<bool, sf::Vector2i>> middle_points;

	for (const auto& l : root->GetRoom()) {
		rooms.push_back(l->leaf_data);
		middle_points.push_back({ false, sf::Vector2i(l->leaf_data.left + l->leaf_data.width / 2, l->leaf_data.top + l->leaf_data.height / 2) });
	}

	delete root;
	delete now_map_;
	temp_leafs.clear();

	now_map_ = new Map();
	now_map_->grid = std::vector(kSpaceSizeHeight, std::vector(kSpaceSizeWight, std::array<int, kLayerCount>()));
	now_map_->block_map = std::vector(kSpaceSizeHeight, std::vector(kSpaceSizeWight, std::set<Sprite*>()));

	for (const auto& rect : rooms) {
		for (const auto& i : std::vector<std::pair<int, int>>{ {kFloorLaye, 1}, {kCellLayer, 2} }) {
			WriteRoom(rect, i.first, Random::IntRandom(1, kTextureCount) + kTextureCount * i.second);
		}

		WriteRoom(rect, kWallLayer, Random::IntRandom(2, kTextureCount));

		sf::IntRect smal_rect({ rect.left + 1, rect.top + 1 }, { rect.width - 2, rect.height - 2 });
		WriteRoom(smal_rect, kWallLayer, 0);
	}

	bool is_all = false;

	int current = Random::IntRandom(0, middle_points.size() - 1);
	start_position_ = (sf::Vector2f)middle_points[current].second;
	middle_points[current].first = true;

	while (!is_all) {
		float min_dist = INFINITY;
		int min_room = -1;

		for (int i = 0; i < middle_points.size(); i++) {
			if (!middle_points[i].first) {
				float dist = GETDIST(middle_points[i].second, middle_points[current].second);

				if (min_dist > dist) {
					min_dist = dist;
					min_room = i;
				}
			}
		}

		sf::Vector2i direction = middle_points[current].second - middle_points[min_room].second;

		float angle = atan2(-direction.y, direction.x) * 180 / kPI;
		angle = fmod(angle, 360.0f);
		if (angle < 0) { angle += 360.0f; }

		sf::IntRect rect = rooms[min_room];
		if ((angle >= 315.0f && angle < 360.0f) || (angle >= 0.0f && angle < 45.0f)) {
			WriteHall(sf::Vector2i(rect.left + rect.width, rect.top + rect.height / 2), false);
		} else if (angle >= 45.0f && angle < 135.0f) {
			WriteHall(sf::Vector2i(rect.left + rect.width / 2, rect.top), true);
		} else if (angle >= 135.0f && angle < 225.0f) {
			WriteHall(sf::Vector2i(rect.left, rect.top + rect.height / 2), false);
		} else {
			WriteHall(sf::Vector2i(rect.left + rect.width / 2, rect.top + rect.height), true);
		}

		current = min_room;
		middle_points[current].first = true;

		is_all = true;
		for (const auto& pair : middle_points) {
			if (!pair.first) {
				is_all = false;
				break;
			}
		}

		end_position_ = (sf::Vector2f)middle_points[current].second;
	}

	for (size_t y = 0; y < now_map_->grid.size(); y++) {
		for (size_t x = 0; x < now_map_->grid[0].size(); x++) {
			if (now_map_->grid[y][x][1] > 1) {
				if (Random::BitRandom() > 0.9f) {
					now_map_->grid[y][x][kWallLayer] = Random::IntRandom(1, kTextureCount) + kTextureCount * 3;
				}
			}
		}
	}

	std::vector<sf::IntRect> enemy_rooms = rooms;

	for (size_t i = 0; i < enemy_rooms.size(); i++) {
		if (enemy_rooms[i].contains((sf::Vector2i)start_position_)) {
			enemy_rooms.erase(enemy_rooms.begin() + i);
			break;
		}
	}

	WriteEnemy(enemy_rooms);
}

void MapManager::WriteRoom(const sf::IntRect& rect, int layer, int value) {
	for (size_t y = rect.top; y < rect.top + rect.height; y++) {
		for (size_t x = rect.left; x < rect.left + rect.width; x++) {
			now_map_->grid[y][x][layer] = value;
		}
	}
}

void MapManager::WriteEnemy(const std::vector<sf::IntRect>& rooms) {
	auto& state = GameState::GetInstance();
	int midle_room_count = std::min(kEnemyLevelCount, (state.data.level_number + 1) * 7) / (float)rooms.size();
	int min_enemy = std::min((int)((state.data.level_number + 1) * 0.5f), kEnemyMaxIndex - 5);
	int max_enemy = std::min((int)((state.data.level_number + 1) * 1.2f), kEnemyMaxIndex);

	for (auto r : rooms) {
		r.top += 2;
		r.left += 2;
		r.height -= 4;
		r.width -= 4;

		auto points = Random::UniquePoints(r, Random::IntRandom((int)(midle_room_count * 0.8f),
			(int)(midle_room_count * 1.2f)));

		for (const auto& p : points) {
			auto index = Random::IntRandom(min_enemy, max_enemy);
			auto def = sprite_defs[index];
			MapSprite map_sprite = { def.texture + 1, (sf::Vector2f)p, (float)Random::IntRandom(0,180), enemy_defs[index].max_healpoint };
			now_map_->SetMapSprite(map_sprite);
		}
	}

	MapSprite map_sprite = { kPortalIndex, end_position_, 0.0f, 100.0f };
	now_map_->SetMapSprite(map_sprite);
}

void MapManager::WriteHall(sf::Vector2i start_pos, bool is_vertical) {
	if (is_vertical) {
		for (int y = -3; y < 4; y++) {
			now_map_->grid[start_pos.y + y][start_pos.x][kWallLayer] = 0;
		}
	} else {
		for (int x = -3; x < 4; x++) {
			now_map_->grid[start_pos.y][start_pos.x + x][kWallLayer] = 0;
		}
	}

	now_map_->grid[start_pos.y][start_pos.x][kWallLayer] = 1;
}

Map* const MapManager::GetNowMap() const { return now_map_; }
