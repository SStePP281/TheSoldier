#include "SpriteManager.h"
#include "SFML/Graphics/ConvexShape.hpp"

#define DOT(a,b) (a.x * b.x + a.y * b.y)

SpriteManager::SpriteManager(Map* _nowMap, UIManager* _uiManager, ItemManager* _itemManager) : 
	nowMap{ _nowMap }, uiManager{ _uiManager }, itemManager{ _itemManager }, id {1}
{
	allSprites = new std::vector<std::shared_ptr<Sprite>>();
	Init();

	auto& event = EventSystem::GetInstance();
	event.Subscribe<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", [=](const std::pair<int, sf::Vector2i> pair) { SpawnEnemy(pair); });

	event.Subscribe<sf::Vector2f>("SPAWN_PORTAL", [&](const sf::Vector2f pos) {SpawnPortal(pos);});
}

void SpriteManager::Init()
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

	auto& data = Data::GetInstance();
	PlayerDef plDef = data.GetPlayerData();

	if (!player)
	{
		bool flag = false;
		for (auto sp : nowMap->GetMapSprites())
		{
			if (sp.spriteDefId == 0)
			{
				auto def = spriteDefs[sp.spriteDefId];
				CreatePlayer(sp, def, plDef);
				flag = true;
				break;
			}
		}

		if (!flag)
		{
			CreateDefaultPlayer(plDef);
		}
	}


	for (auto sp : nowMap->GetMapSprites()) {
		CreateSpriteFromMapSprite(sp);
	}
}

void SpriteManager::CreateSpriteFromMapSprite(const MapSprite& mapSprite)
{
	auto def = spriteDefs[mapSprite.spriteDefId];
	if (def.type == SpriteType::Enemy)
	{
		CreateEnemy(mapSprite, def);
	}
	else if (def.type == SpriteType::NPC)
	{
		CreateNpc(mapSprite, def);
	}
	else if (def.type == SpriteType::Convertor)
	{
		CreateConverter(mapSprite, def);
	}
	else if (def.type == SpriteType::Boss)
	{
		CreateBoss(mapSprite, def);
	}
	else if (def.type == SpriteType::Decoration)
	{
		CreateDecor(mapSprite, def);
	}
	
	id++;
}

void SpriteManager::CreateDecor(const MapSprite& mapSprite, const SpriteDef& spDef)
{
	allSprites->push_back(std::make_shared<Sprite>(spDef, mapSprite, id));
}

void SpriteManager::CreateBoss(const MapSprite& spMap, const SpriteDef& spDef)
{
	auto enemy = enemyDefs[spMap.spriteDefId];
	auto cDef = converterDefs[spDef.texture - kEnemyMaxIndex];
	auto boss = std::make_shared<Boss>(Boss(spDef, spMap, enemy, cDef, id));
	if (spMap.nowHealPoint <= 0.0f)
	{
		boss->ChangeState(EnemyState::Dead);
	}
	else
	{
		boss->ChangeState(EnemyState::Attack);
		enemys.push_back(boss.get());
		nowMap->SetupBlockmap(boss.get());
	}

	allSprites->push_back(std::move(boss));
}

void SpriteManager::CreateConverter(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto cDef = converterDefs[def.texture - kEnemyMaxIndex];
	auto converter = std::make_shared<Converter>(Converter(def, mapSprite, enemyDefs[mapSprite.spriteDefId], cDef, id));

	if (converter->map_sprite.nowHealPoint <= 0.0f) { return; }
	
	converter->ChangeState(EnemyState::Attack);
	enemys.push_back(converter.get());
	nowMap->SetupBlockmap(converter.get());
	allSprites->push_back(std::move(converter));
}

void SpriteManager::CreateEnemy(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto enemy = std::make_shared<Enemy>(def, mapSprite, enemyDefs[mapSprite.spriteDefId], id);
	
	if (enemy->map_sprite.nowHealPoint <= 0.0f)
	{
		enemy->ChangeState(EnemyState::Dead);
	}
	else
	{
		enemy->ChangeState(EnemyState::Attack);
		enemys.push_back(enemy.get());
		nowMap->SetupBlockmap(enemy.get());
	}

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::CreateNpc(const MapSprite& mapSprite, const SpriteDef& def)
{
	auto npcDef = npcDefs[def.texture - kEnemyMaxIndex - 4];

	if (npcDef.type == NpcType::TraderNpcType)
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
	else if (npcDef.type == NpcType::Traveler)
	{
		allSprites->push_back(std::make_shared<TravelerNpc>(TravelerNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == NpcType::ChangerNpcType)
	{
		allSprites->push_back(std::make_shared<ChangerNpc>(ChangerNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == NpcType::PortalNpcType)
	{
		allSprites->push_back(std::make_shared<PortalNpc>(PortalNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == NpcType::MechanicNpcType)
	{
		allSprites->push_back(std::make_shared<MechanicNpc>(MechanicNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else if (npcDef.type == NpcType::QuestNpcType)
	{
		allSprites->push_back(std::make_shared<QuestNpc>(QuestNpc(def, mapSprite, npcDef, uiManager, itemManager, player.get(), id)));
	}
	else
	{
		allSprites->push_back(std::make_shared<Npc>(Npc(def, mapSprite, uiManager, player.get(), npcDef, id)));
	}

	nowMap->SetupBlockmap(allSprites->back().get());
}

void SpriteManager::CreatePlayer(const MapSprite& mapSprite, const SpriteDef& def, const PlayerDef& plDef)
{
	auto enemy = std::make_shared<Enemy>(def, mapSprite, enemyDefs[mapSprite.spriteDefId], id);

	enemy->id = 0;
	enemy->enemy_def.maxHealpoint = plDef.maxHp;
	enemy->map_sprite.nowHealPoint = plDef.nowHp;
	player = std::make_unique<Player>(enemy.get(), plDef, nowMap);
	player->patrons = plDef.countpantrons;
	nowMap->SetupBlockmap(enemy.get());

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::CreateDefaultPlayer(const PlayerDef& plDef)
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
	nowMap->SetupBlockmap(enemy.get());

	player = std::make_unique<Player>(enemy.get(), plDef, nowMap);
	player->patrons = plDef.countpantrons;

	allSprites->push_back(std::move(enemy));
}

void SpriteManager::ResetMap(Map* newMap, const sf::Vector2f& pos)
{
	nowMap = newMap;
	Init();

	if (pos.x != 0.0f && pos.y != 0)
	{
		player->enemy->map_sprite.position = pos;
	}
	player->SetNemMap(newMap);
}

void SpriteManager::ResetOldPlayer()
{
	auto& data = Data::GetInstance();
	PlayerDef plDef = data.GetPlayerData();

	player->enemy->enemy_def.maxHealpoint = plDef.maxHp;
	player->enemy->map_sprite.nowHealPoint = plDef.nowHp;
	player->max_energy = plDef.maxEnergy;
	player->now_energy = plDef.nowEnergy;
	player->defence = plDef.defence;
	player->max_strenght = plDef.maxStrenght;
	player->now_strenght = plDef.nowStrenght;
	player->patrons = plDef.countpantrons;
	player->money = plDef.money;
	player->details = plDef.details;
}

Player* SpriteManager::GetPlayer() { return player.get(); }

Npc* SpriteManager::GetNpc(int id)
{
	auto npc = std::find_if(allSprites->begin(), allSprites->end(), 
		[id](std::shared_ptr<Sprite> sp) {return sp->id == id;});
	return npc != allSprites->end() ? dynamic_cast<Npc*>(npc->get()) : nullptr;
}

std::vector<std::shared_ptr<Sprite>>* SpriteManager::GetDeteachSprite() { return allSprites;  }

void SpriteManager::Update(float deltaTime)
{
	for (auto enemy : enemys)
	{
		enemy->Update(deltaTime);

		if (enemy->map_sprite.nowHealPoint <= 0.0f)
		{
			if (enemy->sprite_def.type == SpriteType::Boss)
			{
				enemy->ChangeState(EnemyState::Dead);
			}
			else
			{
				KillEnemy(enemy);
			}
		}
	}

	AIControler(deltaTime);
}

void SpriteManager::AIControler(float deltaTime)
{
	for (size_t i = 0; i < enemys.size();i++)
	{
		float distance = sqrt(GETDIST(enemys[i]->map_sprite.position, player->enemy->map_sprite.position));

		if (enemys[i]->is_attack && enemys[i]->is_can_attack)
		{
			enemys[i]->is_attack = false;
			enemys[i]->is_can_attack = false;
			if (IsEnemyHit(enemys[i], distance)) enemys[i]->Attack(player.get());

			if (player->enemy->map_sprite.nowHealPoint <= 0.0f)
			{
				auto& event = EventSystem::GetInstance();
				event.Trigger<int>("PLAYERDEAD", 0);
				break;
			}
		}

		sf::Vector2f toPlayerDir = player->enemy->map_sprite.position - enemys[i]->map_sprite.position;
		enemys[i]->EnemyMechenic(distance, toPlayerDir, nowMap, deltaTime);
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

bool SpriteManager::IsEnemyHit(Enemy* enemy, float distance)
{
	float angle = enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	RayHit hit = Raycast(nowMap, enemy->map_sprite.position, dir, false, enemy, distance);
	if (hit.cell != 0) return false;

	return isPointInAttackRect(
		player->enemy->map_sprite.position,
		enemy->map_sprite.position,
		dir,
		enemy->enemy_def.attackDist
	);
}

void SpriteManager::SpawnEnemy(const std::pair<int, sf::Vector2i>& pair)
{
	int x0 = std::max(pair.second.x - kSpawnRadius, 0), x1 = std::min(pair.second.x + kSpawnRadius, (int)nowMap->block_map[0].size());
	int y0 = std::max(pair.second.y - kSpawnRadius, 0), y1 = std::min(pair.second.y + kSpawnRadius, (int)nowMap->block_map.size());

	std::vector<sf::Vector2i> posVec;

	for (int x = x0; x < x1; x++)
	{
		for (int y = y0; y < y1; y++)
		{
			if (nowMap->block_map[y][x].size() == 0) { posVec.push_back({ x,y }); }
		}
	}

	auto index = Random::IntRandom(0, posVec.size() - 1);
	auto enemyDef = enemyDefs[pair.first];
	auto spDef = spriteDefs[pair.first];
	MapSprite spMap = {spDef.texture + 1, (sf::Vector2f)posVec[index], -90.0f, enemyDef.maxHealpoint};

	CreateEnemy(spMap, spDef);
	id++;
}

void SpriteManager::SpawnPortal(const sf::Vector2f& pos)
{
	auto def = spriteDefs[kPortalIndex];
	MapSprite spMap{ def.texture + 1, pos, -90.0f, 10 };
	CreateNpc(spMap, def);
}

void SpriteManager::KillEnemy(Enemy* enem)
{
	enem->ChangeState(EnemyState::Dead);

	int details = Random::IntRandom((int)(enem->enemy_def.midleDrop * 0.8f), (int)(enem->enemy_def.midleDrop * 1.2f));
	player->details += details;
	auto& questM = QuestManager::GetInstance();
	questM.UpdateQuests(QuestType::CollectionDetails, details);
	questM.UpdateQuests(QuestType::KillMonster, 1);

	nowMap->DeleteInBlockMap(enem);

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