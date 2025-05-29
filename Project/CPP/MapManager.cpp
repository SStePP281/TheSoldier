#include "MapManager.h"

MapManager::MapManager(sf::RenderWindow* _window) : 
	window{ _window }, nowMap{ nullptr }
{
	auto& event = EventSystem::getInstance();
	event.subscribe<int>("SAVE", [=](const int NON) { save(); });

	event.subscribe<int>("RESET_GAME", [&](const int NON) {
		auto& state = GameState::getInstance(); 
		state.data.isLevelBase = true; 
		state.data.levelNumber = 0;
		event.trigger<int>("SWAPLOC", BASE_N);
		});

	event.subscribe<int>("WIN_GAME", [&](const int NON) {
		load(mapFileNames[BASE_N]);
		auto& state = GameState::getInstance();
		state.data.isLevelBase = true;
		state.data.levelNumber = 0;
		event.trigger<int>("SWAPLOC", 0);
		});
}

MapManager::~MapManager()
{
	delete nowMap;
}

void MapManager::save()
{
	std::ofstream out{ "Data/current.map", std::ios::out | std::ios::binary};
	if (!out.is_open()) return;
	if (nowMap->grid.empty()) return;

	auto grid = nowMap->grid;
	size_t h = grid.size();
	size_t w = grid[0].size();

	out.write(reinterpret_cast<char*>(&w), sizeof(w));
	out.write(reinterpret_cast<const char*>(&h), sizeof(h));

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			out.write(reinterpret_cast<char*>(grid[y][x].data()),
				sizeof(grid[y][x][0]) * LAYER_COUNT);
		}
	}

	auto sprites = nowMap->sprites;
	size_t numSp = sprites.size();
	out.write(reinterpret_cast<char*>(&numSp), sizeof(numSp));

	for (size_t i = 0; i < sprites.size(); i++)
	{
		out.write(reinterpret_cast<char*>(&sprites[i]), sizeof(sprites[i]));
	}

	out.flush();
	out.close();
}

void MapManager::load(std::string fileName)
{
	std::ifstream in;
	if (fileName == "") { in = std::ifstream{ "Data/current.map", std::ios::in | std::ios::binary }; }
	else
	{
		fileName = "Data/" + fileName;
		in = std::ifstream{ fileName, std::ios::in | std::ios::binary };
	}
	if (!in.is_open()) return;

	nowMap = new Map();

	size_t h = 0, w = 0;
	in.read(reinterpret_cast<char*>(&w), sizeof(w));
	in.read(reinterpret_cast<char*>(&h), sizeof(h));

	if (h == 0 && w == 0)
	{
		h = SPACE_SIZE_H;
		w = SPACE_SIZE_W;
	}

	nowMap->grid = std::vector(h, std::vector(w, std::array<int, LAYER_COUNT>()));
	nowMap->blockMap = std::vector(h, std::vector(w, std::set<Sprite*>()));

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			in.read(reinterpret_cast<char*>(nowMap->grid[y][x].data()),
				sizeof(nowMap->grid[y][x][0]) * LAYER_COUNT);
		}
	}

	size_t numSp;
	in.read(reinterpret_cast<char*>(&numSp), sizeof(numSp));

	nowMap->sprites = std::vector<MapSprite>(numSp);
	for (size_t i = 0; i < nowMap->sprites.size(); i++)
		in.read(reinterpret_cast<char*>(&nowMap->sprites[i]), sizeof(nowMap->sprites[i]));

	in.close();
}

sf::Vector2f MapManager::nextLocation(int index)
{
	auto& state = GameState::getInstance();
	if (index == BASE_N)
	{
		state.data.isLevelBase = true;
		SoundManager::playerMusic(BaseSound);
		load(mapFileNames[BASE_N]);
		endPos = {0.0f, 0.0f};
		for (auto sp : nowMap->sprites)
		{
			if (sp.spriteDefId == 0)
			{
				startPos = sp.position;
				break;
			}
		}
	}
	else
	{
		state.data.isLevelBase = false;
		SoundManager::playerMusic(LevelSound);
		if (index == NEXT_LEVEL_N)
		{
			state.data.levelNumber++;
			generate();
		}
		else
		{
			load(mapFileNames[index]);
			for (auto sp : nowMap->sprites)
			{
				if (sp.spriteDefId == 0)
				{
					startPos = sp.position;
					break;
				}
			}

		}
	}

	return startPos;
}

void MapManager::rewriteSprites(std::vector<std::shared_ptr<Sprite>>* sprs)
{
	nowMap->sprites.clear();
	for (auto sp : *sprs) { nowMap->sprites.push_back(sp->spMap); }
}

sf::Vector2f MapManager::getStartPosition() { return startPos; }

void MapManager::drawMap(int layerNumber)
{
	if (nowMap->grid.empty()) return;

	sf::RectangleShape cell(sf::Vector2f(TEXTURE_SIZE * 0.95f, TEXTURE_SIZE * 0.95f));
	cell.setTexture(&Resources::textures);

	if (layerNumber != SPRITE_LAYER)
	{
		for (size_t y = 0; y < nowMap->grid.size(); y++)
		{
			for (size_t x = 0; x < nowMap->grid[y].size(); x++)
			{
				int init = nowMap->grid[y][x][layerNumber];
				if (init != 0)
				{
					cell.setTextureRect(sf::IntRect(sf::Vector2i((init - 1) % TEXTURE_COUNT * TEXTURE_SIZE,
						(init - 1) / TEXTURE_COUNT * TEXTURE_SIZE),
						sf::Vector2i(TEXTURE_SIZE, TEXTURE_SIZE)));
					cell.setPosition((float)TEXTURE_SIZE * (sf::Vector2f((float)x, (float)y) + sf::Vector2f(0.025f, 0.025f)));
					window->draw(cell);
				}
			}
		}
	}
	else
	{
		cell.setFillColor(sf::Color(255, 255, 255, 100));
		for (size_t y = 0; y < nowMap->grid.size(); y++)
		{
			for (size_t x = 0; x < nowMap->grid[y].size(); x++)
			{
				int init = nowMap->grid[y][x][WALL_LAYER];
				if (init != 0)
				{
					cell.setTextureRect(sf::IntRect(sf::Vector2i((init - 1) % TEXTURE_COUNT * TEXTURE_SIZE,
						(init - 1) / TEXTURE_COUNT * TEXTURE_SIZE),
						sf::Vector2i(TEXTURE_SIZE, TEXTURE_SIZE)));
					cell.setPosition((float)TEXTURE_SIZE * (sf::Vector2f((float)x, (float)y) + sf::Vector2f(0.025f, 0.025f)));
					window->draw(cell);
				}
			}
		}

		sf::RectangleShape spShape(sf::Vector2f(ICON_SIZE, ICON_SIZE));
		spShape.setTexture(&Resources::spriteIcon);

		for (auto sp : nowMap->sprites)
		{
			if (sp.spriteDefId != 0)
			{
				spShape.setTextureRect(sf::IntRect(sf::Vector2i(ICON_SIZE * (sp.spriteDefId - 1), 0),
					sf::Vector2i(ICON_SIZE, ICON_SIZE)));
				spShape.setOrigin(spShape.getLocalBounds().width / 2, spShape.getLocalBounds().height / 2);
				spShape.setPosition((float)TEXTURE_SIZE * sp.position);
				spShape.rotate(sp.angle);
				window->draw(spShape);
			}
		}
	}
}

void MapManager::generate()
{
	Leaf* root = new Leaf({ 0,0 }, { SPACE_SIZE_W, SPACE_SIZE_H });
	std::vector<Leaf*> tempLeaf;
	tempLeaf.push_back(root);

	bool didSplit = true;
	while (didSplit)
	{
		didSplit = false;
		for (int i = 0; i < tempLeaf.size(); i++)
		{
			if (!tempLeaf[i]->leftChild && !tempLeaf[i]->rightChild)
			{
				if (tempLeaf[i]->leafData.width > MAX_LEAF_SIZE || tempLeaf[i]->leafData.height > MAX_LEAF_SIZE || Random::bitRandom() > 0.25)
				{
					if (tempLeaf[i]->split())
					{
						tempLeaf.push_back(tempLeaf[i]->leftChild);
						tempLeaf.push_back(tempLeaf[i]->rightChild);
						didSplit = true;
					}
				}
			}
		}
	}

	root->findRoom();
	root->getAllChild();

	std::vector<sf::IntRect> rooms;

	std::vector<std::pair<bool, sf::Vector2i>> middlePoint;
	for (auto l : root->getRoom())
	{
		rooms.push_back(l->leafData);
		middlePoint.push_back({ false, sf::Vector2i(l->leafData.left + l->leafData.width / 2, l->leafData.top + l->leafData.height / 2) });
	}

	delete root;
	delete nowMap;

	nowMap = new Map();
	nowMap->grid = std::vector(SPACE_SIZE_H, std::vector(SPACE_SIZE_W, std::array<int, LAYER_COUNT>()));
	nowMap->blockMap = std::vector(SPACE_SIZE_H, std::vector(SPACE_SIZE_W, std::set<Sprite*>()));

	for (auto rect : rooms)
	{
		for (auto i : std::vector<std::pair<int, int>>{ {FLOOR_LAYER, 1}, {CELL_LAYER, 2} })
		{
			writeRoom(rect, i.first, Random::intRandom(1, TEXTURE_COUNT) + TEXTURE_COUNT * i.second);
		}
		writeRoom(rect, WALL_LAYER, Random::intRandom(2, TEXTURE_COUNT));

		sf::IntRect smalRect({ rect.left + 1, rect.top + 1 }, { rect.width - 2, rect.height - 2 });
		writeRoom(smalRect, WALL_LAYER, 0);
	}

	bool isAll = false;
	int current = Random::intRandom(0, middlePoint.size() - 1);
	startPos = (sf::Vector2f)middlePoint[current].second;
	middlePoint[current].first = true;
	while (!isAll)
	{
		float minDist = INFINITY;
		int minRoom = -1;

		for (int i = 0; i < middlePoint.size(); i++)
		{
			if (!middlePoint[i].first)
			{
				float dist = GETDIST(middlePoint[i].second, middlePoint[current].second);

				if (minDist > dist)
				{
					minDist = dist;
					minRoom = i;
				}
			}
		}

		sf::Vector2i dir = middlePoint[current].second - middlePoint[minRoom].second;
		float angle = atan2(-dir.y, dir.x) * 180 / PI;
		angle = fmod(angle, 360.0f);
		if (angle < 0) angle += 360.0f;

		sf::IntRect rect = rooms[minRoom];
		if ((angle >= 315.0f && angle < 360.0f) ||
			(angle >= 0.0f && angle < 45.0f))
		{
			nowMap->grid[rect.top + rect.height / 2][rect.left + rect.width - 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height / 2][rect.left + rect.width - 1][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height / 2][rect.left + rect.width][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height / 2][rect.left + rect.width][WALL_LAYER] = 1;
		}
		else if (angle >= 45.0f && angle < 135.0f)
		{
			nowMap->grid[rect.top][rect.left + rect.width / 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top][rect.left + rect.width / 2][WALL_LAYER] = 1;
			nowMap->grid[rect.top - 1][rect.left + rect.width / 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top - 2][rect.left + rect.width / 2][WALL_LAYER] = 0;
		}
		else if (angle >= 135.0f && angle < 225.0f)
		{
			nowMap->grid[rect.top + rect.height / 2][rect.left][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height / 2][rect.left][WALL_LAYER] = 1;
			nowMap->grid[rect.top + rect.height / 2][rect.left - 1][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height / 2][rect.left - 2][WALL_LAYER] = 0;
		}
		else
		{
			nowMap->grid[rect.top + rect.height - 2][rect.left + rect.width / 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height - 1][rect.left + rect.width / 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height][rect.left + rect.width / 2][WALL_LAYER] = 0;
			nowMap->grid[rect.top + rect.height][rect.left + rect.width / 2][WALL_LAYER] = 1;
		}

		current = minRoom;
		middlePoint[current].first = true;

		isAll = true;
		for (auto pair : middlePoint)
		{
			if (!pair.first)
			{
				isAll = false;
				break;
			}
		}

		endPos = (sf::Vector2f)middlePoint[current].second;
	}

	for (size_t y = 0; y < nowMap->grid.size(); y++)
	{
		for (size_t x = 0; x < nowMap->grid[0].size(); x++)
		{
			if (nowMap->grid[y][x][1] > 1)
			{
				if (Random::bitRandom() > 0.9f)
				{
					nowMap->grid[y][x][WALL_LAYER] = Random::intRandom(1, TEXTURE_COUNT) + TEXTURE_COUNT * 3;
				}
			}
		}
	}

	std::vector<sf::IntRect> enemyRooms = rooms;
	for (size_t i = 0; i < enemyRooms.size(); i++)
	{
		if (enemyRooms[i].contains((sf::Vector2i)startPos))
		{
			enemyRooms.erase(enemyRooms.begin() + i);
			break;
		}
	}

	writeEnemy(enemyRooms);
}

void MapManager::writeRoom(const sf::IntRect& rect, int layer, int value)
{
	for (size_t y = rect.top; y < rect.top + rect.height; y++)
	{
		for (size_t x = rect.left; x < rect.left + rect.width; x++)
		{
			nowMap->grid[y][x][layer] = value;
		}
	}
}

void MapManager::writeEnemy(const std::vector<sf::IntRect>& rooms)
{
	auto& state = GameState::getInstance();
	int midleRoomCount = std::min(ENEMY_LEVEL_COUNT, (state.data.levelNumber + 1) * 7) / (float)rooms.size();
	int minEnemy = std::min((int)((state.data.levelNumber + 1) * 0.5f), ENEMY_MAX_INDEX - 5);
	int maxEnemy = std::min((int)((state.data.levelNumber + 1) * 1.2f), ENEMY_MAX_INDEX);

	for (auto r : rooms)
	{
		r.top += 2;
		r.left += 2;
		r.height -= 4;
		r.width -= 4;

		auto points = Random::uniquePoints(r, Random::intRandom((int)(midleRoomCount * 0.8f),
			(int)(midleRoomCount * 1.2f)));

		for (auto p : points)
		{
			auto index = Random::intRandom(minEnemy, maxEnemy);
			auto def = spriteDefs[index];
			MapSprite spMap = { def.texture + 1, (sf::Vector2f)p, (float)Random::intRandom(0,180), enemyDefs[index].maxHealpoint };
			nowMap->setMapSprite(spMap);
		}
	}

	MapSprite spMap = { PORTAL_INDEX, endPos, 0.0f, 100.0f };
	nowMap->setMapSprite(spMap);
}

Map* MapManager::getNowMap() { return nowMap; }
