#include "SpriteManager.h"
#include "SFML/Graphics/ConvexShape.hpp"

#define DOT(a,b) (a.x * b.x + a.y * b.y)

SpriteManager::SpriteManager(Map* _nowMap, UIManager* _uiManager, ItemManager* _itemManager) : 
	nowMap{ _nowMap }, uiManager{ _uiManager }, itemManager{ _itemManager }, id {1}
{
	allSprites = new std::vector<std::shared_ptr<Sprite>>();
	init();

	auto& event = EventSystem::getInstance();
	event.subscribe<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", [=](const std::pair<int, sf::Vector2i> pair) { spawnEnemy(pair); });

	event.subscribe<sf::Vector2f>("SPAWN_PORTAL", [&](const sf::Vector2f pos) {spawnPortal(pos);});
}

void SpriteManager::init()
{
	id = 1;
	int i = 0;
	while (i < allSprites->size())
	{
		if ((*allSprites)[i]->id != 0)
		{
			(*allSprites)[i].reset();
			allSprites->erase(allSprites->begin() + i);
		}
		else
		{
			i++;
		}
	}

	enemys.clear();

	auto& data = Data::getInstance();
	PlayerDef plDef = data.getPlayerData();

	if (!player)
	{
		bool flag = false;
		for (auto sp : nowMap->getMapSprites())
		{
			if (sp.spriteDefId == 0)
			{
				auto def = spriteDefs[sp.spriteDefId];
				createPlayer(sp, def, plDef);
				flag = true;
				break;
			}
		}

		if (!flag)
		{
			createDefaultPlayer(plDef);
		}
	}


	for (auto sp : nowMap->getMapSprites()) {
		createSpriteFromMapSprite(sp);
	}
}

void SpriteManager::createSpriteFromMapSprite(const MapSprite& mapSprite)
{
	auto def = spriteDefs[mapSprite.spriteDefId];
	if (def.type == SpriteType::Enemy)
	{
		createEnemy(mapSprite, def);
	}
	else if (def.type == SpriteType::NPC)
	{
		createNpc(mapSprite, def);
	}
	else if (def.type == SpriteType::Convertor)
	{
		createConverter(mapSprite, def);
	}
	else if (def.type == SpriteType::Boss)
	{
		createBoss(mapSprite, def);
	}
	else if (def.type == SpriteType::Decoration)
	{
		createDecor(mapSprite, def);
	}
	
	id++;
}

void SpriteManager::createDecor(const MapSprite& mapSprite, const SpriteDef& spDef)
{
	allSprites->push_back(std::make_shared<Sprite>(spDef, mapSprite, id));
}

void SpriteManager::createBoss(const MapSprite& spMap, const SpriteDef& spDef)
{
	auto enemy = enemyDefs[spMap.spriteDefId];
	auto cDef = converterDefs[spDef.texture - ENEMY_MAX_INDEX];
	auto boss = std::make_shared<Boss>(Boss(spDef, spMap, enemy, cDef, id));
	if (spMap.nowHealPoint <= 0.0f)
	{
		boss->changeState(Dead);
	}
	else
	{
		boss->changeState(Attack);
		enemys.push_back(boss.get());
		nowMap->setupBlockmap(boss.get());
	}

	allSprites->push_back(std::move(boss));
}

void SpriteManager::createConverter(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto cDef = converterDefs[def.texture - ENEMY_MAX_INDEX];
	auto converter = std::make_shared<Converter>(Converter(def, mapSprite, enemyDefs[mapSprite.spriteDefId], cDef, id));

	if (converter->spMap.nowHealPoint <= 0.0f) { return; }
	
	converter->changeState(Attack);
	enemys.push_back(converter.get());
	nowMap->setupBlockmap(converter.get());
	allSprites->push_back(std::move(converter));
}

void SpriteManager::createEnemy(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto enemy = std::make_shared<Enemy>(def, mapSprite, enemyDefs[mapSprite.spriteDefId], id);
	
	if (enemy->spMap.nowHealPoint <= 0.0f)
	{
		enemy->changeState(Dead);
	}
	else
	{
		enemy->changeState(Attack);
		enemys.push_back(enemy.get());
		nowMap->setupBlockmap(enemy.get());
	}

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::createNpc(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto npcDef = npcDefs[def.texture - ENEMY_MAX_INDEX - 4];

	if (npcDef.type == TraderNpcType)
	{
		TraderDef tradeDef;
		for (auto t : traderDefs)
		{
			if (t.startKey == npcDef.idKey)
			{
				tradeDef = t;
				break;
			}
		}

		allSprites->push_back(std::make_shared<TradeNpc>(TradeNpc(def, mapSprite, tradeDef, npcDef, itemManager, 
			uiManager, player.get(), id)));
	}
	else if (npcDef.type == Traveler)
	{
		allSprites->push_back(std::make_shared<TravelerNpc>(TravelerNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == ChangerNpcType)
	{
		allSprites->push_back(std::make_shared<ChangerNpc>(ChangerNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == PortalNpcType)
	{
		allSprites->push_back(std::make_shared<PortalNpc>(PortalNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == MechanicNpcType)
	{
		allSprites->push_back(std::make_shared<MechanicNpc>(MechanicNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == QuestNpcType)
	{
		allSprites->push_back(std::make_shared<QuestNpc>(QuestNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else
	{
		allSprites->push_back(std::make_shared<Npc>(Npc(def, mapSprite, uiManager, player.get(), npcDef, id)));
	}

	nowMap->setupBlockmap(allSprites->back().get());
}

void SpriteManager::createPlayer(const MapSprite& mapSprite, const SpriteDef& def, const PlayerDef& plDef)
{
	auto enemy = std::make_shared<Enemy>(def, mapSprite, enemyDefs[mapSprite.spriteDefId], id);

	enemy->id = 0;
	enemy->enemyDef.maxHealpoint = plDef.maxHp;
	enemy->spMap.nowHealPoint = plDef.nowHp;
	player = std::make_unique<Player>(enemy.get(), plDef, nowMap);
	player->patrons = plDef.countpantrons;
	nowMap->setupBlockmap(enemy.get());

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::createDefaultPlayer(const PlayerDef& plDef)
{
	auto def = spriteDefs[0];
	auto enDef = enemyDefs[0];
	enDef.maxHealpoint = plDef.maxHp;

	MapSprite defaultPlayerSprite{
		0,                              
		{2.0f, 2.0f},                   
		90.0f,                          
		plDef.nowHp                     
	};

	auto enemy = std::make_shared<Enemy>(def, defaultPlayerSprite, enDef, 0);
	nowMap->setupBlockmap(enemy.get());

	player = std::make_unique<Player>(enemy.get(), plDef, nowMap);
	player->patrons = plDef.countpantrons;

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::resetMap(Map* newMap, const sf::Vector2f& pos)
{
	nowMap = newMap;
	init();

	if (pos.x != 0.0f && pos.y != 0)
	{
		player->enemy->spMap.position = pos;
	}
	player->setNemMap(newMap);
}

void SpriteManager::resetOldPlayer()
{
	auto& data = Data::getInstance();
	PlayerDef plDef = data.getPlayerData();

	player->enemy->enemyDef.maxHealpoint = plDef.maxHp;
	player->enemy->spMap.nowHealPoint = plDef.nowHp;
	player->maxEnergy = plDef.maxEnergy;
	player->nowEnergy = plDef.nowEnergy;
	player->defence = plDef.defence;
	player->maxStrenght = plDef.maxStrenght;
	player->nowStrenght = plDef.nowStrenght;
	player->patrons = plDef.countpantrons;
	player->money = plDef.money;
	player->details = plDef.details;
}

Player* SpriteManager::getPlayer() { return player.get(); }

Npc* SpriteManager::getNpc(int id)
{
	auto npc = std::find_if(allSprites->begin(), allSprites->end(), 
		[id](std::shared_ptr<Sprite> sp) {return sp->id == id;});
	return npc != allSprites->end() ? dynamic_cast<Npc*>(npc->get()) : nullptr;
}

std::vector<std::shared_ptr<Sprite>>* SpriteManager::getDeteachSprite() { return allSprites;  }

void SpriteManager::update(float deltaTime)
{
	for (auto enemy : enemys)
	{
		enemy->update(deltaTime);

		if (enemy->spMap.nowHealPoint <= 0.0f)
		{
			if (enemy->spDef.type == SpriteType::Boss)
			{
				enemy->changeState(Dead);
			}
			else
			{
				killEnemy(enemy);
			}
		}
	}

	aiControler(deltaTime);
}

void SpriteManager::aiControler(float deltaTime)
{
	for (size_t i = 0; i < enemys.size();i++)
	{
		float distance = sqrt(GETDIST(enemys[i]->spMap.position, player->enemy->spMap.position));

		if (enemys[i]->isAtack && enemys[i]->isCanAttack)
		{
			enemys[i]->isAtack = false;
			enemys[i]->isCanAttack = false;
			if (isEnemyHit(enemys[i], distance)) enemys[i]->attack(player.get());

			if (player->enemy->spMap.nowHealPoint <= 0.0f)
			{
				auto& event = EventSystem::getInstance();
				event.trigger<int>("PLAYERDEAD", 0);
				break;
			}
		}

		sf::Vector2f toPlayerDir = player->enemy->spMap.position - enemys[i]->spMap.position;
		enemys[i]->enemyMechenic(distance, toPlayerDir, nowMap, deltaTime);
	}
}

bool isPointInAttackRect(const sf::Vector2f& point, const sf::Vector2f& pos,
	sf::Vector2f dir, float attackDist)
{
	sf::Vector2f rel = point - pos;

	float projForward = DOT(rel, dir);
	float projLeft = DOT(rel, sf::Vector2f(-dir.y, dir.x));

	return (projForward >= 0.0f && projForward <= attackDist + 1.0f &&
		projLeft >= -1.0f / 2.0f && projLeft <= 1.0f / 2.0f);
}

bool SpriteManager::isEnemyHit(Enemy* enemy, float distance)
{
	float angle = enemy->spMap.angle * PI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	RayHit hit = raycast(nowMap, enemy->spMap.position, dir, false, enemy, distance);
	if (hit.cell != 0) return false;

	return isPointInAttackRect(
		player->enemy->spMap.position,
		enemy->spMap.position,
		dir,
		enemy->enemyDef.attackDist
	);
}

void SpriteManager::spawnEnemy(const std::pair<int, sf::Vector2i>& pair)
{
	int x0 = std::max(pair.second.x - SPAWN_RADIUS, 0), x1 = std::min(pair.second.x + SPAWN_RADIUS, (int)nowMap->blockMap[0].size());
	int y0 = std::max(pair.second.y - SPAWN_RADIUS, 0), y1 = std::min(pair.second.y + SPAWN_RADIUS, (int)nowMap->blockMap.size());

	std::vector<sf::Vector2i> posVec;

	for (int x = x0; x < x1; x++)
	{
		for (int y = y0; y < y1; y++)
		{
			if (nowMap->blockMap[y][x].size() == 0) { posVec.push_back({ x,y }); }
		}
	}

	auto index = Random::intRandom(0, posVec.size() - 1);
	auto enemyDef = enemyDefs[pair.first];
	auto spDef = spriteDefs[pair.first];
	MapSprite spMap = {spDef.texture + 1, (sf::Vector2f)posVec[index], -90.0f, enemyDef.maxHealpoint};

	createEnemy(spMap, spDef);
	id++;
}

void SpriteManager::spawnPortal(const sf::Vector2f& pos)
{
	auto def = spriteDefs[PORTAL_INDEX];
	MapSprite spMap{ def.texture + 1, pos, -90.0f, 10 };
	createNpc(spMap, def);
}

void SpriteManager::killEnemy(Enemy* enem)
{
	enem->changeState(Dead);

	int details = Random::intRandom((int)(enem->enemyDef.midleDrop * 0.8f), (int)(enem->enemyDef.midleDrop * 1.2f));
	player->details += details;
	auto& questM = QuestManager::getInstance();
	questM.updateQuests(CollectionDetails, details);
	questM.updateQuests(KillMonster, 1);

	nowMap->deleteInBlockMap(enem);

	for (int i = 0; i < enemys.size(); i++)
	{
		if (enemys[i]->id == enem->id)
		{
			enemys.erase(enemys.begin() + i);
			break;
		}
	}
}

SpriteManager::~SpriteManager()
{
	delete allSprites;
}