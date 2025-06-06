#include "SpriteManager.h"

SpriteManager::SpriteManager(Map* now_map, UIManager* ui_manager, ItemManager* item_manager) : 
	now_map{ now_map }, ui_manager{ ui_manager }, item_manager{ item_manager }, id {1},
	all_sprites{ new std::vector<std::shared_ptr<Sprite>>() }
{
	Init();

	auto& event = EventSystem::GetInstance();
	event.Subscribe<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", [=](const std::pair<int, sf::Vector2i> pair) { SpawnEnemy(pair); });

	event.Subscribe<sf::Vector2f>("SPAWN_PORTAL", [&](const sf::Vector2f pos) {SpawnPortal(pos);});
}

void SpriteManager::Init()
{
	id = 1;
	int i = 0;
	while (i < all_sprites->size())
	{
		if ((*all_sprites)[i]->id != 0)
		{
			(*all_sprites)[i].reset();
			all_sprites->erase(all_sprites->begin() + i);
		}
		else
		{
			i++;
		}
	}

	enemys.clear();

	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	if (!player)
	{
		bool flag = false;
		for (auto& map_sprite : now_map->GetMapSprites())
		{
			if (map_sprite.spriteDefId == 0)
			{
				auto sprite_def = spriteDefs[map_sprite.spriteDefId];
				CreatePlayer(map_sprite, sprite_def, player_def);
				flag = true;
				break;
			}
		}

		if (!flag)
		{
			CreateDefaultPlayer(player_def);
		}
	}


	for (auto& map_sprite : now_map->GetMapSprites()) {
		CreateSpriteFromMapSprite(map_sprite);
	}
}

void SpriteManager::CreateSpriteFromMapSprite(const MapSprite& map_sprite)
{
	auto sprite_def = spriteDefs[map_sprite.spriteDefId];
	if (sprite_def.type == SpriteType::Enemy)
	{
		CreateEnemy(map_sprite, sprite_def);
	}
	else if (sprite_def.type == SpriteType::NPC)
	{
		CreateNpc(map_sprite, sprite_def);
	}
	else if (sprite_def.type == SpriteType::Convertor)
	{
		CreateConverter(map_sprite, sprite_def);
	}
	else if (sprite_def.type == SpriteType::Boss)
	{
		CreateBoss(map_sprite, sprite_def);
	}
	else if (sprite_def.type == SpriteType::Decoration)
	{
		CreateDecor(map_sprite, sprite_def);
	}
	
	id++;
}

void SpriteManager::CreateDecor(const MapSprite& map_sprite, const SpriteDef& sprite_def)
{
	all_sprites->push_back(std::make_shared<Sprite>(sprite_def, map_sprite, id));
}

void SpriteManager::CreateBoss(const MapSprite& map_sprite, const SpriteDef& sprite_def)
{
	auto enemy_def = enemyDefs[map_sprite.spriteDefId];
	auto converter_def = converterDefs[sprite_def.texture - kEnemyMaxIndex];
	auto boss = std::make_shared<Boss>(Boss(sprite_def, map_sprite, enemy_def, converter_def, id));

	if (map_sprite.nowHealPoint <= 0.0f)
	{
		boss->ChangeState(EnemyState::Dead);
	}
	else
	{
		boss->ChangeState(EnemyState::Attack);
		enemys.push_back(boss.get());
		now_map->SetupBlockmap(boss.get());
	}

	all_sprites->push_back(std::move(boss));
}

void SpriteManager::CreateConverter(const MapSprite& map_sprite, const SpriteDef& sprite_def)
{
	if (map_sprite.nowHealPoint <= 0.0f) { return; }

	auto converter_def = converterDefs[sprite_def.texture - kEnemyMaxIndex];
	auto converter = std::make_shared<Converter>(Converter(sprite_def, map_sprite, enemyDefs[map_sprite.spriteDefId], converter_def, id));

	converter->ChangeState(EnemyState::Attack);
	enemys.push_back(converter.get());
	now_map->SetupBlockmap(converter.get());
	all_sprites->push_back(std::move(converter));
}

void SpriteManager::CreateEnemy(const MapSprite& map_sprite, const SpriteDef& sprite_def)
{
	auto enemy = std::make_shared<Enemy>(sprite_def, map_sprite, enemyDefs[map_sprite.spriteDefId], id);
	
	if (enemy->map_sprite.nowHealPoint <= 0.0f)
	{
		enemy->ChangeState(EnemyState::Dead);
	}
	else
	{
		enemy->ChangeState(EnemyState::Attack);
		enemys.push_back(enemy.get());
		now_map->SetupBlockmap(enemy.get());
	}

	all_sprites->push_back(std::move(enemy));
}

void SpriteManager::CreateNpc(const MapSprite& map_sprite, const SpriteDef& sprite_def)
{
	auto npc_def = npcDefs[sprite_def.texture - kEnemyMaxIndex - 4];

	if (npc_def.type == NpcType::TraderNpcType)
	{
		TraderDef trade_def;
		for (auto& t : traderDefs)
		{
			if (t.startKey == npc_def.idKey)
			{
				trade_def = t;
				break;
			}
		}

		all_sprites->push_back(std::make_shared<TradeNpc>(TradeNpc(sprite_def, map_sprite, trade_def, npc_def, item_manager, 
			ui_manager, player.get(), id)));
	}
	else if (npc_def.type == NpcType::Traveler)
	{
		all_sprites->push_back(std::make_shared<TravelerNpc>(TravelerNpc(sprite_def, map_sprite, npc_def, ui_manager, item_manager, player.get(), id)));
	}
	else if (npc_def.type == NpcType::ChangerNpcType)
	{
		all_sprites->push_back(std::make_shared<ChangerNpc>(ChangerNpc(sprite_def, map_sprite, npc_def, ui_manager, item_manager, player.get(), id)));
	}
	else if (npc_def.type == NpcType::PortalNpcType)
	{
		all_sprites->push_back(std::make_shared<PortalNpc>(PortalNpc(sprite_def, map_sprite, npc_def, ui_manager, item_manager, player.get(), id)));
	}
	else if (npc_def.type == NpcType::MechanicNpcType)
	{
		all_sprites->push_back(std::make_shared<MechanicNpc>(MechanicNpc(sprite_def, map_sprite, npc_def, ui_manager, item_manager, player.get(), id)));
	}
	else if (npc_def.type == NpcType::QuestNpcType)
	{
		all_sprites->push_back(std::make_shared<QuestNpc>(QuestNpc(sprite_def, map_sprite, npc_def, ui_manager, item_manager, player.get(), id)));
	}
	else
	{
		all_sprites->push_back(std::make_shared<Npc>(Npc(sprite_def, map_sprite, ui_manager, player.get(), npc_def, id)));
	}

	now_map->SetupBlockmap(all_sprites->back().get());
}

void SpriteManager::CreatePlayer(const MapSprite& map_sprite, const SpriteDef& sprite_def, const PlayerDef& player_def)
{
	auto enemy = std::make_shared<Enemy>(sprite_def, map_sprite, enemyDefs[map_sprite.spriteDefId], id);

	enemy->id = 0;
	enemy->enemy_def.maxHealpoint = player_def.maxHp;
	enemy->map_sprite.nowHealPoint = player_def.nowHp;
	player = std::make_unique<Player>(enemy.get(), player_def, now_map);
	player->patrons = player_def.countpantrons;
	now_map->SetupBlockmap(enemy.get());

	all_sprites->push_back(std::move(enemy));
}

void SpriteManager::CreateDefaultPlayer(const PlayerDef& player_def)
{
	auto sprite_def = spriteDefs[0];
	auto enemy_def = enemyDefs[0];
	enemy_def.maxHealpoint = player_def.maxHp;

	MapSprite default_player_sprite{
		0,                              
		{2.0f, 2.0f},                   
		90.0f,                          
		player_def.nowHp                     
	};

	auto enemy = std::make_shared<Enemy>(sprite_def, default_player_sprite, enemy_def, 0);
	now_map->SetupBlockmap(enemy.get());

	player = std::make_unique<Player>(enemy.get(), player_def, now_map);
	player->patrons = player_def.countpantrons;

	all_sprites->push_back(std::move(enemy));
}

void SpriteManager::ResetMap(Map* new_map, const sf::Vector2f& position)
{
	now_map = new_map;
	Init();

	if (position.x != 0.0f && position.y != 0)
	{
		player->enemy->map_sprite.position = position;
	}
	player->SetNemMap(new_map);
}

void SpriteManager::ResetOldPlayer()
{
	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	player->enemy->enemy_def.maxHealpoint = player_def.maxHp;
	player->enemy->map_sprite.nowHealPoint = player_def.nowHp;
	player->max_energy = player_def.maxEnergy;
	player->now_energy = player_def.nowEnergy;
	player->defence = player_def.defence;
	player->max_strenght = player_def.maxStrenght;
	player->now_strenght = player_def.nowStrenght;
	player->patrons = player_def.countpantrons;
	player->money = player_def.money;
	player->details = player_def.details;
}

Player* SpriteManager::GetPlayer() { return player.get(); }

Npc* SpriteManager::GetNpc(int id)
{
	auto npc = std::find_if(all_sprites->begin(), all_sprites->end(), 
		[id](std::shared_ptr<Sprite> sp) {return sp->id == id;});
	return npc != all_sprites->end() ? dynamic_cast<Npc*>(npc->get()) : nullptr;
}

std::vector<std::shared_ptr<Sprite>>* SpriteManager::GetDeteachSprite() { return all_sprites;  }

void SpriteManager::Update(float delta_time)
{
	for (auto& enemy : enemys)
	{
		enemy->Update(delta_time);

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

	AIControler(delta_time);
}

void SpriteManager::AIControler(float delta_time)
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

		sf::Vector2f to_player_dir = player->enemy->map_sprite.position - enemys[i]->map_sprite.position;
		enemys[i]->EnemyMechenic(distance, to_player_dir, now_map, delta_time);
	}
}

bool isPointInAttackRect(const sf::Vector2f& point, const sf::Vector2f& position,
	const sf::Vector2f& direction, float attack_dist)
{
	sf::Vector2f rel = point - position;

	float proj_forward = DOT(rel, direction);
	float proj_left = DOT(rel, sf::Vector2f(-direction.y, direction.x));

	return (proj_forward >= 0.0f && proj_forward <= attack_dist + 1.0f &&
		proj_left >= -1.0f / 2.0f && proj_left <= 1.0f / 2.0f);
}

bool SpriteManager::IsEnemyHit(Enemy* enemy, float distance)
{
	float angle = enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	RayHit hit = Raycast(now_map, enemy->map_sprite.position, dir, false, enemy, distance);
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
	int x0 = std::max(pair.second.x - kSpawnRadius, 0), x1 = std::min(pair.second.x + kSpawnRadius, (int)now_map->block_map[0].size());
	int y0 = std::max(pair.second.y - kSpawnRadius, 0), y1 = std::min(pair.second.y + kSpawnRadius, (int)now_map->block_map.size());

	std::vector<sf::Vector2i> pos_vec;

	for (int x = x0; x < x1; x++)
	{
		for (int y = y0; y < y1; y++)
		{
			if (now_map->block_map[y][x].size() == 0) { pos_vec.push_back({ x,y }); }
		}
	}

	auto index = Random::IntRandom(0, pos_vec.size() - 1);
	auto enemy_def = enemyDefs[pair.first];
	auto sprite_def = spriteDefs[pair.first];
	MapSprite map_sprite = {sprite_def.texture + 1, (sf::Vector2f)pos_vec[index], -90.0f, enemy_def.maxHealpoint};

	CreateEnemy(map_sprite, sprite_def);
	id++;
}

void SpriteManager::SpawnPortal(const sf::Vector2f& pos)
{
	auto def = spriteDefs[kPortalIndex];
	MapSprite map_sprite{ def.texture + 1, pos, -90.0f, 10 };
	CreateNpc(map_sprite, def);
}

void SpriteManager::KillEnemy(Enemy* enem)
{
	enem->ChangeState(EnemyState::Dead);

	int details = Random::IntRandom((int)(enem->enemy_def.midleDrop * 0.8f), (int)(enem->enemy_def.midleDrop * 1.2f));
	player->details += details;
	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.UpdateQuests(QuestType::CollectionDetails, details);
	quest_manager.UpdateQuests(QuestType::KillMonster, 1);

	now_map->DeleteInBlockMap(enem);

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
	delete all_sprites;
}