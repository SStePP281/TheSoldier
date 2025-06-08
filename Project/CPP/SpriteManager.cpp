#include "sprite_manager.h"

SpriteManager::SpriteManager(Map* const now_map, UIManager* const ui_manager, ItemManager* const item_manager) :
	now_map_{ now_map }, ui_manager_{ ui_manager }, item_manager_{ item_manager }, id_ {1},
	all_sprites_{ new std::vector<std::shared_ptr<Sprite>>() } {
	Init();

	auto& event = EventSystem::GetInstance();
	event.Subscribe<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", [=](const std::pair<int, sf::Vector2i> pair) { SpawnEnemy(pair); });

	event.Subscribe<sf::Vector2f>("SPAWN_PORTAL", [&](const sf::Vector2f pos) {SpawnPortal(pos);});
}

void SpriteManager::Init() {
	id_ = 1;
	int i = 0;
	while (i < all_sprites_->size()) {
		if ((*all_sprites_)[i]->GetId() != 0) {
			(*all_sprites_)[i].reset();
			all_sprites_->erase(all_sprites_->begin() + i);
		} else {
			i++;
		}
	}

	enemys_.clear();

	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	if (!player_) {
		bool flag = false;
		for (const auto& map_sprite : now_map_->GetMapSprites()) {
			if (map_sprite.sprite_def_id == 0) {
				auto sprite_def = sprite_defs[map_sprite.sprite_def_id];
				CreatePlayer(map_sprite, sprite_def, player_def);
				flag = true;
				break;
			}
		}

		if (!flag) {
			CreateDefaultPlayer(player_def);
		}
	}


	for (const auto& map_sprite : now_map_->GetMapSprites()) {
		CreateSpriteFromMapSprite(map_sprite);
	}
}

void SpriteManager::CreateSpriteFromMapSprite(const MapSprite& map_sprite) {
	auto sprite_def = sprite_defs[map_sprite.sprite_def_id];
	if (sprite_def.type == SpriteType::Enemy) {
		CreateEnemy(map_sprite, sprite_def);
	} else if (sprite_def.type == SpriteType::NPC) {
		CreateNpc(map_sprite, sprite_def);
	} else if (sprite_def.type == SpriteType::Convertor) {
		CreateConverter(map_sprite, sprite_def);
	} else if (sprite_def.type == SpriteType::Boss) {
		CreateBoss(map_sprite, sprite_def);
	} else if (sprite_def.type == SpriteType::Decoration) {
		CreateDecor(map_sprite, sprite_def);
	}
	
	id_++;
}

void SpriteManager::CreateDecor(const MapSprite& map_sprite, const SpriteDef& sprite_def) {
	all_sprites_->push_back(std::make_shared<Sprite>(sprite_def, map_sprite, id_));
}

void SpriteManager::CreateBoss(const MapSprite& map_sprite, const SpriteDef& sprite_def) {
	auto enemy_def = enemy_defs[map_sprite.sprite_def_id];
	auto converter_def = converter_defs[sprite_def.texture - kEnemyMaxIndex];
	auto boss = std::make_shared<Boss>(Boss(sprite_def, map_sprite, enemy_def, converter_def, id_));

	if (map_sprite.now_heal_point <= 0.0f) {
		boss->ChangeState(EnemyState::Dead);
	} else {
		boss->ChangeState(EnemyState::Attack);
		enemys_.push_back(boss.get());
		now_map_->SetupBlockmap(boss.get());
	}

	all_sprites_->push_back(std::move(boss));
}

void SpriteManager::CreateConverter(const MapSprite& map_sprite, const SpriteDef& sprite_def) {
	if (map_sprite.now_heal_point <= 0.0f) { return; }

	auto converter_def = converter_defs[sprite_def.texture - kEnemyMaxIndex];
	auto converter = std::make_shared<Converter>(Converter(sprite_def, map_sprite, enemy_defs[map_sprite.sprite_def_id], converter_def, id_));

	converter->ChangeState(EnemyState::Attack);
	enemys_.push_back(converter.get());
	now_map_->SetupBlockmap(converter.get());
	all_sprites_->push_back(std::move(converter));
}

void SpriteManager::CreateEnemy(const MapSprite& map_sprite, const SpriteDef& sprite_def) {
	auto enemy = std::make_shared<Enemy>(sprite_def, map_sprite, enemy_defs[map_sprite.sprite_def_id], id_);
	
	if (enemy->GetHealpoint() <= 0.0f) {
		enemy->ChangeState(EnemyState::Dead);
	} else {
		enemy->ChangeState(EnemyState::Attack);
		enemys_.push_back(enemy.get());
		now_map_->SetupBlockmap(enemy.get());
	}

	all_sprites_->push_back(std::move(enemy));
}

void SpriteManager::CreateNpc(const MapSprite& map_sprite, const SpriteDef& sprite_def) {
	auto npc_def = npc_defs[sprite_def.texture - kEnemyMaxIndex - 4];

	if (npc_def.type == NpcType::TraderNpcType) {
		TraderDef trade_def;
		for (const auto& t : trader_defs) {
			if (t.start_key == npc_def.id_key) {
				trade_def = t;
				break;
			}
		}

		all_sprites_->push_back(std::make_shared<TradeNpc>(TradeNpc(sprite_def, map_sprite, trade_def, npc_def, item_manager_, 
			ui_manager_, player_.get(), id_)));
	} else if (npc_def.type == NpcType::Traveler) {
		all_sprites_->push_back(std::make_shared<TravelerNpc>(TravelerNpc(sprite_def, map_sprite, npc_def, ui_manager_, item_manager_, player_.get(), id_)));
	} else if (npc_def.type == NpcType::ChangerNpcType) {
		all_sprites_->push_back(std::make_shared<ChangerNpc>(ChangerNpc(sprite_def, map_sprite, npc_def, ui_manager_, item_manager_, player_.get(), id_)));
	} else if (npc_def.type == NpcType::PortalNpcType) {
		all_sprites_->push_back(std::make_shared<PortalNpc>(PortalNpc(sprite_def, map_sprite, npc_def, ui_manager_, item_manager_, player_.get(), id_)));
	} else if (npc_def.type == NpcType::MechanicNpcType) {
		all_sprites_->push_back(std::make_shared<MechanicNpc>(MechanicNpc(sprite_def, map_sprite, npc_def, ui_manager_, item_manager_, player_.get(), id_)));
	} else if (npc_def.type == NpcType::QuestNpcType) {
		all_sprites_->push_back(std::make_shared<QuestNpc>(QuestNpc(sprite_def, map_sprite, npc_def, ui_manager_, item_manager_, player_.get(), id_)));
	} else {
		all_sprites_->push_back(std::make_shared<Npc>(Npc(sprite_def, map_sprite, ui_manager_, player_.get(), npc_def, id_)));
	}

	now_map_->SetupBlockmap(all_sprites_->back().get());
}

void SpriteManager::CreatePlayer(const MapSprite& map_sprite, const SpriteDef& sprite_def, const PlayerDef& player_def) {
	auto enemy = std::make_shared<Enemy>(sprite_def, map_sprite, enemy_defs[map_sprite.sprite_def_id], 0);

	enemy->SetMaxHealPoint(player_def.max_healpoint);
	enemy->SetHealpoint(player_def.now_healpoint);
	player_ = std::make_unique<Player>(enemy.get(), player_def, now_map_);
	player_->SetPatrons(player_def.patrons_count);
	now_map_->SetupBlockmap(enemy.get());

	all_sprites_->push_back(std::move(enemy));
}

void SpriteManager::CreateDefaultPlayer(const PlayerDef& player_def) {
	auto sprite_def = sprite_defs[0];
	auto enemy_def = enemy_defs[0];
	enemy_def.max_healpoint = player_def.max_healpoint;

	MapSprite default_player_sprite{
		0,                              
		{2.0f, 2.0f},                   
		90.0f,                          
		player_def.now_healpoint                     
	};

	auto enemy = std::make_shared<Enemy>(sprite_def, default_player_sprite, enemy_def, 0);
	now_map_->SetupBlockmap(enemy.get());

	player_ = std::make_unique<Player>(enemy.get(), player_def, now_map_);
	player_->SetPatrons(player_def.patrons_count);

	all_sprites_->push_back(std::move(enemy));
}

void SpriteManager::ResetMap(Map* const new_map, const sf::Vector2f& position) {
	now_map_ = new_map;
	Init();

	if (position.x != 0.0f && position.y != 0) {
		player_->GetEnemy()->SetPosition(position);
	}

	player_->SetNemMap(new_map);
}

void SpriteManager::ResetOldPlayer() {
	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	player_->ResetPlayer(player_def);
}

Player* const SpriteManager::GetPlayer() const { return player_.get(); }

Npc* const SpriteManager::GetNpc(int id) const {
	auto npc = std::find_if(all_sprites_->begin(), all_sprites_->end(), 
		[id](std::shared_ptr<Sprite> sp) {return sp->GetId() == id;});

	return npc != all_sprites_->end() ? dynamic_cast<Npc*>(npc->get()) : nullptr;
}

std::vector<std::shared_ptr<Sprite>>* const SpriteManager::GetDeteachSprite() const { return all_sprites_;  }

void SpriteManager::Update(float delta_time) {
	for (const auto& enemy : enemys_) {
		enemy->Update(delta_time);

		if (enemy->GetHealpoint() <= 0.0f) {
			if (enemy->GetSpriteType() == SpriteType::Boss) {
				enemy->ChangeState(EnemyState::Dead);
			} else {
				KillEnemy(enemy);
			}
		}
	}

	AIControler(delta_time);
}

void SpriteManager::AIControler(float delta_time) const {
	for (size_t i = 0; i < enemys_.size();i++) {
		float distance = sqrt(GETDIST(enemys_[i]->GetPosition(), player_->GetEnemy()->GetPosition()));

		if (enemys_[i]->GetIsAttack() && enemys_[i]->GetIsCanAttack()) {
			enemys_[i]->SetIsAttcak(false);
			enemys_[i]->SetIsCanAttcak(false);
			if (IsEnemyHit(enemys_[i], distance)) enemys_[i]->Attack(player_.get());

			if (player_->GetEnemy()->GetHealpoint() <= 0.0f) {
				auto& event = EventSystem::GetInstance();
				event.Trigger<int>("PLAYERDEAD", 0);
				break;
			}
		}

		sf::Vector2f to_player_dir = player_->GetEnemy()->GetPosition() - enemys_[i]->GetPosition();
		enemys_[i]->EnemyMechenic(distance, to_player_dir, now_map_, delta_time);
	}
}

bool isPointInAttackRect(const sf::Vector2f& point, const sf::Vector2f& position,
	const sf::Vector2f& direction, float attack_dist) {
	sf::Vector2f rel = point - position;

	float proj_forward = DOT(rel, direction);
	float proj_left = DOT(rel, sf::Vector2f(-direction.y, direction.x));

	return (proj_forward >= 0.0f && proj_forward <= attack_dist + 1.0f &&
		proj_left >= -1.0f / 2.0f && proj_left <= 1.0f / 2.0f);
}

bool SpriteManager::IsEnemyHit(Enemy* const enemy, float distance) const {
	float angle = enemy->GetAngle() * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	RayHit hit = Raycast(now_map_, enemy->GetPosition(), dir, false, enemy, distance);
	if (hit.cell != 0) return false;

	return isPointInAttackRect(
		player_->GetEnemy()->GetPosition(),
		enemy->GetPosition(),
		dir,
		enemy->GetAttackDistance()
	);
}

void SpriteManager::SpawnEnemy(const std::pair<int, sf::Vector2i>& pair) {
	int x0 = std::max(pair.second.x - kSpawnRadius, 0), x1 = std::min(pair.second.x + kSpawnRadius, (int)now_map_->block_map[0].size());
	int y0 = std::max(pair.second.y - kSpawnRadius, 0), y1 = std::min(pair.second.y + kSpawnRadius, (int)now_map_->block_map.size());

	std::vector<sf::Vector2i> pos_vec;

	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			if (now_map_->block_map[y][x].size() == 0) { 
				pos_vec.push_back({ x,y });
			}
		}
	}

	auto index = Random::IntRandom(0, pos_vec.size() - 1);
	auto enemy_def = enemy_defs[pair.first];
	auto sprite_def = sprite_defs[pair.first];
	MapSprite map_sprite = {sprite_def.texture + 1, (sf::Vector2f)pos_vec[index], -90.0f, enemy_def.max_healpoint};

	CreateEnemy(map_sprite, sprite_def);
	id_++;
}

void SpriteManager::SpawnPortal(const sf::Vector2f& pos) {
	auto def = sprite_defs[kPortalIndex];
	MapSprite map_sprite{ def.texture + 1, pos, -90.0f, 10 };
	CreateNpc(map_sprite, def);
}

void SpriteManager::KillEnemy(Enemy* const enem) {
	enem->ChangeState(EnemyState::Dead);

	int details = Random::IntRandom((int)(enem->GetMidleDrop() * 0.8f), (int)(enem->GetMidleDrop() * 1.2f));
	player_->SetDetails(player_->GetDetails() + details);
	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.UpdateQuests(QuestType::CollectionDetails, details);
	quest_manager.UpdateQuests(QuestType::KillMonster, 1);

	now_map_->DeleteInBlockMap(enem);

	for (int i = 0; i < enemys_.size(); i++) {
		if (enemys_[i]->GetId() == enem->GetId()) {
			enemys_.erase(enemys_.begin() + i);
			break;
		}
	}
}

SpriteManager::~SpriteManager() {
	delete all_sprites_;
}