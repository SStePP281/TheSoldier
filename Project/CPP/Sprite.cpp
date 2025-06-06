#include "Sprite.h"
#include "UIManeger.h"
#include "ItemManager.h"
#include "Map.h"
#include "Weapon.h"
#include "Player.h"

Sprite::Sprite(const SpriteDef& sprite_def, const MapSprite& map_sprite, int id) :
	sprite_def{ sprite_def }, map_sprite{ map_sprite }, id{ id }, texture_size{ 0 }, texture{ nullptr }
{
	if (sprite_def.type == SpriteType::Decoration)
	{
		texture = &Resources::sprites_textures[sprite_def.texture];
		texture_size = texture->getSize().y;
	}
	else if (sprite_def.texture != -1)
	{
		texture = &Resources::sprites_textures[sprite_def.texture];
		texture_size = texture->getSize().x / 8;
	}
}

std::pair<int, bool> Sprite::GetTextIndex() { return { 0, false }; }

Enemy::Enemy(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, int id) :
	Sprite(sprite_def, map_sprite, id), enemy_def{ enemy_def }, state{ EnemyState::Stay }, time_atecked{ 0.5f }, is_damaged{ false }, now_time_attack{ 0 }
{
	float frame_time = 1.0f / enemy_def.speed;
	auto stay = Animation<int>({ {0,0} });

	int index = 1;
	Animation<int> run;
	if (enemy_def.isCanRun)
	{
		run = Animation<int>({
		{ frame_time * 0, index++ },
		{ frame_time * 1, index++ },
		{ frame_time * 2, index++ },
		{ frame_time * 3, index },
		{ frame_time * 4, index++ } });
	}
	else
	{
		run = stay;
	}

	frame_time = 1.0f / 3;

	auto atack = Animation<int>({
	{ frame_time * 0, index++ },
	{ frame_time * 1, index++ },
	{ frame_time * 2, index },
	{ frame_time * 3, index++ } });

	auto dead = Animation<int>({ {0, index} });

	animr = Animator<int>(0, { stay, run, atack, dead });
}

std::pair<int, bool> Enemy::GetTextIndex() { return { animr.Get(), is_damaged }; }

void Enemy::Death()
{
	animr.SetAnimation(3);
	is_damaged = false;
}

void Enemy::Attack(Player* player)
{
	player->TakeDamage(enemy_def.damage);
}

void Enemy::Move(Map* map, const sf::Vector2f& move)
{
	if (move == sf::Vector2f()) return;

	float x_offset = move.x > 0 ? sprite_def.size / 2.0f : -sprite_def.size / 2.0f;
	float y_offset = move.y > 0 ? sprite_def.size / 2.0f : -sprite_def.size / 2.0f;
	sf::Vector2f delta_x = { map_sprite.position.x + move.x + x_offset, map_sprite.position.y };
	sf::Vector2f delta_y = { map_sprite.position.x, map_sprite.position.y + move.y + y_offset };

	if (!CheckCollision(map, delta_x, true))
	{
		map_sprite.position.x += move.x;
	}
	if (!CheckCollision(map, delta_y, false))
	{
		map_sprite.position.y += move.y;
	}

	map->SetupBlockmap(this);
}

void Enemy::Update(float delta_time)
{
	if (state == EnemyState::Dead) return;

	animr.Update(delta_time);

	UpdateTimeSinceLastAttack(delta_time);
	UpdateTimeSinceDamaged(delta_time);
}

void Enemy::UpdateTimeSinceLastAttack(float delta_time)
{
	if (now_time_attack >= enemy_def.timeBettwenAtack)
	{
		is_can_attack = true;
	}
	else
	{
		now_time_attack += delta_time;
	}
}

void Enemy::UpdateTimeSinceDamaged(float delta_time)
{
	if (time_atecked < 0.5f)
	{
		time_atecked += delta_time;
	}
	else
	{
		is_damaged = false;
	}
}

bool Enemy::CanChangeState()
{
	return animr.Get() == 0 || animr.is_lopping == true;
}

void Enemy::ChangeState(EnemyState new_state)
{
	if (state == EnemyState::Dead) return;

	if (new_state == EnemyState::Stay)
	{
		animr.SetAnimation(0);
	}
	else if (new_state == EnemyState::Run)
	{
		if (state != EnemyState::Run)
		{
			animr.SetAnimation(1, true);
		}
	}
	else if (new_state == EnemyState::Attack)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (new_state == EnemyState::Dead)
	{
		Death();
	}

	state = new_state;
}

void Enemy::TakeDamage(float damage)
{
	if (state == EnemyState::Dead) return;

	map_sprite.nowHealPoint -= damage;
	SoundManager::PlaySounds(Resources::take_damage);

	is_damaged = true;
	time_atecked = 0;
}

EnemyState Enemy::DetermineNewState(float dist)
{
	if (dist < enemy_def.attackDist)
	{
		return EnemyState::Attack;
	}
	else if (dist < kTrigerDist)
	{
		return EnemyState::Run;
	}
	else
	{
		return EnemyState::Stay;
	}
}

void Enemy::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time)
{
	if (is_attack) { return; }
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	}
	else if (new_state == EnemyState::Attack)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(new_state);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	}
	else if (new_state == EnemyState::Stay)
	{
		ChangeState(new_state);
	}
}

bool Enemy::CheckCollision(Map* map, const sf::Vector2f& new_position, bool x_axis)
{
	sf::Vector2f this_size{ sprite_def.size / 2.0f, sprite_def.size / 2.0f };
	sf::Vector2f this_start = new_position - this_size;
	sf::Vector2f this_end = new_position + this_size;

	if (map->GetOnGrid((int)new_position.x, (int)new_position.y, kWallLayer)) { return true; }

	for (auto& sp : map->GetBlockMap((sf::Vector2i)new_position))
	{
		if (sp->sprite_def.size == 0.f || sp == this) continue;

		sf::Vector2f sprite_size = { sp->sprite_def.size / 2.f, sp->sprite_def.size / 2.f };
		sf::Vector2f sprite_start = sp->map_sprite.position - sprite_size;
		sf::Vector2f sprite_end = sp->map_sprite.position + sprite_size;

		if (x_axis)
		{
			float px = new_position.x;
			float py0 = this_start.y, py1 = this_end.y, magnitude = py1 - py0;
			float t1 = (sprite_start.y - py0) / magnitude;
			float t2 = (sprite_end.y - py0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool y_intersect = exit > entry && exit > 0.f && entry < 1.f;
			if (px >= sprite_start.x && px <= sprite_end.x && y_intersect) { return true; }
		}
		else
		{
			float py = new_position.y;
			float px0 = this_start.x, px1 = this_end.x, magnitude = px1 - px0;
			float t1 = (sprite_start.x - px0) / magnitude;
			float t2 = (sprite_end.x - px0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool xIntersect = exit > entry && exit > 0.f && entry < 1.f;
			if (py >= sprite_start.y && py <= sprite_end.y && xIntersect) { return true; }
		}
	}

	return false;
}

Converter::Converter(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& _converter_def, int id) :
	Enemy(sprite_def, map_sprite, enemy_def, id), converter_def{ _converter_def }
{
	now_spawn_count = (int)(converter_def.maxSpawnCount * map_sprite.nowHealPoint / enemy_def.maxHealpoint);
	texture_size = texture->getSize().y / 2;
	Animation<int> stay({ {0.0f, 0} });
	Animation<int> attack({ {0.0f, 0}, {enemy_def.timeBettwenAtack, 0 }, {enemy_def.timeBettwenAtack, 0 } });
	Animation<int> death({ {0.0f, 1} });
	animr = Animator<int>(0, { stay, {}, attack, death });
}

void Converter::TakeDamage(float damege)
{
	if (now_spawn_count != 0) return;

	Enemy::TakeDamage(damege);
}

void Converter::Death()
{
	auto& state = GameState::GetInstance();
	if (sprite_def.texture == 13)
	{
		state.data.killFirst = true;
	}
	else if (sprite_def.texture == 14)
	{
		state.data.killSecond = true;
	}
	else if (sprite_def.texture == 15)
	{
		state.data.killTherd = true;
	}

	is_damaged = false;
	animr.SetAnimation(3);
	auto& event = EventSystem::GetInstance();
	event.Trigger<sf::Vector2f>("SPAWN_PORTAL", map_sprite.position);
}

void Converter::Attack(Player* plaer)
{
	if (now_spawn_count == 0) { return; }
	now_spawn_count--;

	auto& event = EventSystem::GetInstance();
	event.Trigger<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", { converter_def.callingIndex[Random::IntRandom(0, converter_def.callingIndex.size() - 1)], (sf::Vector2i)map_sprite.position });
}

void Converter::ChangeState(EnemyState new_state)
{
	if (state == EnemyState::Dead) return;

	if (new_state == EnemyState::Spawn)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (new_state == EnemyState::Dead)
	{
		Death();
	}

	state = new_state;
}

void Converter::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time)
{
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run && !is_attack)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	}
	else if (new_state == EnemyState::Spawn)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(new_state);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	}
	else if (new_state == EnemyState::Stay)
	{
		ChangeState(new_state);
	}
}

EnemyState Converter::DetermineNewState(float dist)
{
	if (dist < kSpawnRadius)
	{
		return EnemyState::Spawn;
	}
	else if (dist < kTrigerDist)
	{
		return EnemyState::Run;
	}
	else
	{
		return EnemyState::Stay;
	}
}

bool Converter::CanChangeState()
{
	return !is_attack || now_spawn_count > 0;
}

Boss::Boss(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& _converter_def, int id) :
	Enemy(sprite_def, map_sprite, enemy_def, id), converter_def{ _converter_def }
{
	now_spawn_count = (int)(converter_def.maxSpawnCount * map_sprite.nowHealPoint / enemy_def.maxHealpoint);
}

void Boss::Death()
{
	is_damaged = false;
	animr.SetAnimation(3);
	auto& event = EventSystem::GetInstance();
	event.Trigger<sf::Vector2f>("WIN_GAME", map_sprite.position);
}

void Boss::Attack(Player* player)
{
	if (state == EnemyState::Attack)
	{
		Enemy::Attack(player);
	}
	else if (state == EnemyState::Spawn)
	{
		if (now_spawn_count == 0) return;

		now_spawn_count--;

		auto& event = EventSystem::GetInstance();
		event.Trigger<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", { converter_def.callingIndex[Random::IntRandom(0, converter_def.callingIndex.size() - 1)], (sf::Vector2i)map_sprite.position });
	}
}

void Boss::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time)
{
	if (is_attack) { return; }
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	}
	else if (new_state == EnemyState::Spawn || new_state == EnemyState::Attack)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(new_state);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	}
	else if (new_state == EnemyState::Stay)
	{
		ChangeState(new_state);
	}
}

void Boss::ChangeState(EnemyState new_state)
{
	if (state == EnemyState::Dead) return;

	if (new_state == EnemyState::Stay)
	{
		animr.SetAnimation(0);
	}
	else if (new_state == EnemyState::Spawn)
	{
		animr.SetAnimation(0);
		is_attack = true;
		is_can_attack = false;
		now_time_attack = 0.0f;
	}
	else if (new_state == EnemyState::Attack)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (new_state == EnemyState::Run)
	{
		if (state != EnemyState::Run)
		{
			animr.SetAnimation(1, true);
		}
	}
	else if (new_state == EnemyState::Dead)
	{
		Death();
	}

	state = new_state;
}

EnemyState Boss::DetermineNewState(float dist)
{
	int hp_percent = (int)(abs(map_sprite.nowHealPoint - 1) / enemy_def.maxHealpoint * 100.0f);
	int a = hp_percent / 25;
	int b = a % 2;
	if (b == 1)
	{
		if (dist < enemy_def.attackDist)
		{
			return EnemyState::Attack;
		}
		else if (dist < kTrigerDist)
		{
			return EnemyState::Run;
		}
		else
		{
			return EnemyState::Stay;
		}
	}
	else
	{
		if (dist < kSpawnRadius)
		{
			return EnemyState::Spawn;
		}
		else
		{
			return EnemyState::Stay;
		}
	}
}

Npc::Npc(const SpriteDef& sprite_def, const MapSprite& map_sprite, UIManager* ui_manager, Player* player, const NpcDef& npc_def, int id) :
	Sprite(sprite_def, map_sprite, id), npc_def{ npc_def }, player{ player }, ui_manager{ ui_manager }, now_key{ 1 }
{
	texture_size = texture->getSize().y;
}

void Npc::SetEndFunc(std::function<void()>&& _endFunc) { end_func = _endFunc; }

void Npc::Init()
{
	auto& data = Data::GetInstance();
	auto keys = data.GetKeys(npc_def.idKey, now_key);

	std::map<int, std::wstring, std::greater<int>> variants;

	for (int i = 0; i < keys.size(); i++)
	{
		auto d = data.GetText(npc_def.idKey, keys[i]);
		variants[d.second] = d.first;
	}

	ui_manager->InitDialog(variants, sprite_def.name);
}

void Npc::Stop()
{
	now_key = 1;
	end_func();
	end_func = nullptr;
	ui_manager->DeleteNow();
}

void Npc::Use() {}

void Npc::Update(int choose_key)
{
	now_key = choose_key;
	Check();
}

void Npc::Check()
{
	if (now_key == 0)
	{
		Stop();
	}
	else
	{
		ui_manager->DeleteNow();
		Init();
	}
}

FuncNpc::FuncNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, ItemManager* item_manager,
	UIManager* ui_manager, Player* _player, int _id) :
	Npc(sprite_def, map_sprite, ui_manager, _player, npc_def, _id), is_func{ false }, choose{ -1 }, item_manager{ item_manager } {}

void FuncNpc::Stop()
{
	Npc::Stop();

	is_func = false;
	choose = -1;
}

void FuncNpc::Update(int choose_key)
{
	now_key = choose_key;

	if (now_key == 999)
	{
		is_func = true;
		Init();
		return;
	}

	if (!is_func)
	{
		Npc::Check();
	}
	else
	{
		Check();
	}
}

void FuncNpc::Check()
{
	if (now_key == -100)
	{
		Stop();
	}
	else if (now_key == -200)
	{
		Use();
	}
	else
	{
		choose = now_key;
	}
}

TradeNpc::TradeNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const TraderDef& _tradeDef, const NpcDef& npc_def,
	ItemManager* item_manager, UIManager* ui_manager, Player* _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id), trade_def{ _tradeDef } {}

void TradeNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		auto result = std::map<int, Itemble*>();
		for (int i = 0; i < trade_def.title.size(); i++)
		{
			result[trade_def.title[i]] = item_manager->GetItemble(trade_def.title[i]);
		}

		ui_manager->InitTrade(result, player);
	}
}

void TradeNpc::Use()
{
	if (choose == -1) return;

	Itemble* itemble = item_manager->GetItemble(choose);

	if (itemble->cost > player->money) return;

	player->money -= itemble->cost;
	player->TakeItem(itemble);

	choose = -1;

	Init();
}

TravelerNpc::TravelerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* ui_manager, ItemManager* item_manager, Player* _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id) {}

void TravelerNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		auto result = std::map<int, Itemble*>();
		result[0] = item_manager->GetItemble(travelerDefs[0].id);

		int max_size = 1;
		auto& state = GameState::GetInstance();
		int level_number = state.data.levelNumber;
		if (state.data.killFirst &&
			state.data.killSecond &&
			state.data.killTherd)
		{
			max_size = 5;
		}
		else
		{
			max_size = std::min(level_number / 3, 4);
		}

		for (int i = 1; i < max_size; i++)
		{
			result[i] = item_manager->GetItemble(travelerDefs[i].id);
		}

		ui_manager->InitTrade(result, player);
	}
}

void TravelerNpc::Use()
{
	if (choose == -1) return;

	if (travelerDefs[choose].cost > player->money) return;

	player->money -= travelerDefs[choose].cost;

	int temp = travelerDefs[choose].effect;
	Stop();

	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SWAPLOC", temp);
}

ChangerNpc::ChangerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* ui_manager, ItemManager* item_manager, Player* _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id)
{
	auto& state = GameState::GetInstance();
	coef = state.data.changerCoef;
}

void ChangerNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		ui_manager->InitChanger(coef, player);
	}
}

void ChangerNpc::Use()
{
	if (player->details - 10 < 0) return;

	player->details -= 10;
	player->money += 10 * coef;
	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.UpdateQuests(QuestType::CollectionMoney, 10 * coef);

	Init();
}

PortalNpc::PortalNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* ui_manager, ItemManager* item_manager, Player* player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id) {}

void PortalNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		FuncNpc::Stop();

		Use();
	}
}

void PortalNpc::Use()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SWAPLOC", kBaseNumber);
}

MechanicNpc::MechanicNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* ui_manager, ItemManager* item_manager, Player* player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id), type_upgade{ -1 } {}

void MechanicNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		ui_manager->InitMechanic(player, (choose == -1 ? nullptr : player->guns[choose]));
	}
}

void MechanicNpc::Stop()
{
	FuncNpc::Stop();
	type_upgade = -1;
}

void MechanicNpc::Use()
{
	if (choose == -1 || type_upgade == -1) return;

	Gun* now_gun = player->guns[choose];

	if (player->money < 50 || player->details < 15 || now_gun->upgrade_count >= 5) return;

	player->money -= 50;
	player->details -= 15;

	Improve* improve = nullptr;
	if (type_upgade == 101)
	{
		improve = now_gun->DeleteImprove(ImproveType::Damage);
		now_gun->damage += 3;
	}
	else if (type_upgade == 102)
	{
		improve = now_gun->DeleteImprove(ImproveType::Magazin);
		now_gun->max_count += 5;
	}
	else if (type_upgade == 103)
	{
		improve = now_gun->DeleteImprove(ImproveType::Spread);
		now_gun->max_rad += 3;
	}

	now_gun->TrySetImprove(improve);
	now_gun->upgrade_count++;

	type_upgade = -1;
	choose = -1;
	Init();
}

void MechanicNpc::Check()
{
	if (now_key == -100)
	{
		Stop();
	}
	else if (now_key == -200)
	{
		Use();
	}
	else if (now_key == 1 || now_key == 2)
	{
		choose = now_key;
		Init();
		return;
	}
	else
	{
		type_upgade = now_key;
	}
}

QuestNpc::QuestNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* ui_manager, ItemManager* item_manager, Player* player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id) {}

void QuestNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		auto& quest_manager = QuestManager::GetInstance();
		ui_manager->InitQuest((choose == -1 ? nullptr : quest_manager.quests[choose - 1]), player);
	}
}

void QuestNpc::Use()
{
	if (choose == -1) return;

	auto& quest_manager = QuestManager::GetInstance();
	player->money += quest_manager.DeleteQuest(quest_manager.quests[choose - 1]);

	choose = -1;
	Init();
}

void QuestNpc::Check()
{
	if (now_key == -100)
	{
		Stop();
	}
	else if (now_key == -200)
	{
		Use();
	}
	else if (now_key == -300)
	{
		int type = Random::IntRandom(0, 2);
		QuestData quest_data;
		quest_data.progress = 0;
		quest_data.type = QuestType(type);

		if (type == 0)
		{
			quest_data.target = Random::IntRandom(5, 15);
			quest_data.rewardCoins = Random::IntRandom(quest_data.target * 10, quest_data.target * 15);
		}
		else if (type == 1)
		{
			quest_data.target = Random::IntRandom(100, 300);
			quest_data.rewardCoins = Random::IntRandom((int)(quest_data.target * 1.1f), (int)(quest_data.target * 1.3f));
		}
		else if (type == 2)
		{
			quest_data.target = Random::IntRandom(10, 25);
			quest_data.rewardCoins = quest_data.rewardCoins = Random::IntRandom(quest_data.target * 10, quest_data.target * 15);
		}

		auto& quest_manager = QuestManager::GetInstance();
		quest_manager.AddQuest(quest_data);

		Init();
	}
	else if (now_key == 1 || now_key == 2 || now_key == 3)
	{
		choose = now_key;
		Init();
	}
}
