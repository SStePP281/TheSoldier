#include "Sprite.h"
#include "UIManeger.h"
#include "ItemManager.h"
#include "Map.h"
#include "Weapon.h"
#include "Player.h"

Sprite::Sprite(const SpriteDef& _spDef, const MapSprite& _spMap, int _id) :
	sprite_def{ _spDef }, map_sprite{ _spMap }, id{ _id }, texture_size{ 0 }, texture{ nullptr }
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

Enemy::Enemy(const SpriteDef& spDef, const MapSprite& spMap, const EnemyDef& _enemyDef, int id) :
	Sprite(spDef, spMap, id), enemy_def{ _enemyDef }, state{ EnemyState::Stay }, time_atecked{ 0.5f }, is_damaged{ false }, now_time_attack{ 0 }
{
	float frameTime = 1.0f / enemy_def.speed;
	auto stay = Animation<int>({ {0,0} });

	int index = 1;
	Animation<int> run;
	if (enemy_def.isCanRun)
	{
		run = Animation<int>({
		{ frameTime * 0, index++ },
		{ frameTime * 1, index++ },
		{ frameTime * 2, index++ },
		{ frameTime * 3, index },
		{ frameTime * 4, index++ } });
	}
	else
	{
		run = stay;
	}

	frameTime = 1.0f / 3;

	auto atack = Animation<int>({
	{ frameTime * 0, index++ },
	{ frameTime * 1, index++ },
	{ frameTime * 2, index },
	{ frameTime * 3, index++ } });

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

	float xOffset = move.x > 0 ? sprite_def.size / 2.0f : -sprite_def.size / 2.0f;
	float yOffset = move.y > 0 ? sprite_def.size / 2.0f : -sprite_def.size / 2.0f;
	sf::Vector2f deltaX = { map_sprite.position.x + move.x + xOffset, map_sprite.position.y };
	sf::Vector2f deltaY = { map_sprite.position.x, map_sprite.position.y + move.y + yOffset };
	if (!CheckCollision(map, deltaX, true))
	{
		map_sprite.position.x += move.x;
	}
	if (!CheckCollision(map, deltaY, false))
	{
		map_sprite.position.y += move.y;
	}

	map->SetupBlockmap(this);
}

void Enemy::Update(float deltaTime)
{
	if (state == EnemyState::Dead) return;

	animr.Update(deltaTime);

	UpdateTimeSinceLastAttack(deltaTime);
	UpdateTimeSinceDamaged(deltaTime);
}

void Enemy::UpdateTimeSinceLastAttack(float deltaTime)
{
	if (now_time_attack >= enemy_def.timeBettwenAtack)
	{
		is_can_attack = true;
	}
	else
	{
		now_time_attack += deltaTime;
	}
}

void Enemy::UpdateTimeSinceDamaged(float deltaTime)
{
	if (time_atecked < 0.5f)
	{
		time_atecked += deltaTime;
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

void Enemy::ChangeState(EnemyState newState)
{
	if (state == EnemyState::Dead) return;

	if (newState == EnemyState::Stay)
	{
		animr.SetAnimation(0);
	}
	else if (newState == EnemyState::Run)
	{
		if (state != EnemyState::Run)
		{
			animr.SetAnimation(1, true);
		}
	}
	else if (newState == EnemyState::Attack)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (newState == EnemyState::Dead)
	{
		Death();
	}

	state = newState;
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

void Enemy::EnemyMechenic(float dist, const sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime)
{
	if (is_attack) { return; }
	auto newState = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	if (newState == EnemyState::Run)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		sf::Vector2f deltaMove = enemy_def.speed * deltaTime * dir;
		Move(nowMap, deltaMove);

		ChangeState(newState);
	}
	else if (newState == EnemyState::Attack)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(newState);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		}
	}
	else if (newState == EnemyState::Stay)
	{
		ChangeState(newState);
	}
}

bool Enemy::CheckCollision(Map* map, const sf::Vector2f& newPos, bool xAxis)
{
	sf::Vector2f thisSize{ sprite_def.size / 2.0f, sprite_def.size / 2.0f };
	sf::Vector2f thisStart = newPos - thisSize;
	sf::Vector2f thisEnd = newPos + thisSize;
	sf::Vector2i pos(newPos);

	if (xAxis) {
		if (map->GetOnGrid((int)newPos.x, (int)newPos.y, kWallLayer)) { return true; }

		auto curSp = map->GetBlockMap(pos);
		for (auto sp : curSp) 
		{
			if (sp->sprite_def.size == 0.f || sp == this) continue;

			sf::Vector2f spriteSize = { sp->sprite_def.size / 2.f, sp->sprite_def.size / 2.f };
			sf::Vector2f spriteStart = sp->map_sprite.position - spriteSize;
			sf::Vector2f spriteEnd = sp->map_sprite.position + spriteSize;

			float px = newPos.x;
			float py0 = thisStart.y, py1 = thisEnd.y, magnitude = py1 - py0;
			float t1 = (spriteStart.y - py0) / magnitude;
			float t2 = (spriteEnd.y - py0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool yIntersect = exit > entry && exit > 0.f && entry < 1.f;
			if (px >= spriteStart.x && px <= spriteEnd.x && yIntersect) { return true; }
		}
	}
	else {
		if (map->GetOnGrid((int)newPos.x, (int)newPos.y, kWallLayer)) { return true; }

		auto curSp = map->GetBlockMap(pos);
		for (auto sp : curSp) 
		{
			if (sp->sprite_def.size == 0.f || sp == this) continue;

			sf::Vector2f halfSize = { sp->sprite_def.size / 2.f, sp->sprite_def.size / 2.f };
			sf::Vector2f thingStart = sp->map_sprite.position - halfSize;
			sf::Vector2f thingEnd = sp->map_sprite.position + halfSize;

			float py = newPos.y;
			float px0 = thisStart.x, px1 = thisEnd.x, magnitude = px1 - px0;
			float t1 = (thingStart.x - px0) / magnitude;
			float t2 = (thingEnd.x - px0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool xIntersect = exit > entry && exit > 0.f && entry < 1.f;
			if (py >= thingStart.y && py <= thingEnd.y && xIntersect) { return true; }
		}

	}

	return false;
}

Converter::Converter(const SpriteDef& spDef, const MapSprite& spMap, const EnemyDef& enemyDef, const ConverterDef& cDef, int id) :
	Enemy(spDef, spMap, enemyDef, id), converter_def{ cDef }
{
	now_spawn_count = (int)(cDef.maxSpawnCount * spMap.nowHealPoint / enemyDef.maxHealpoint);
	texture_size = texture->getSize().y / 2;
	Animation<int> stay({ {0.0f, 0} });
	Animation<int> attack({ {0.0f, 0}, {enemyDef.timeBettwenAtack, 0 }, {enemyDef.timeBettwenAtack, 0 } });
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

void Converter::ChangeState(EnemyState newState)
{
	if (state == EnemyState::Dead) return;

	if (newState == EnemyState::Spawn)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (newState == EnemyState::Dead)
	{
		Death();
	}

	state = newState;
}

void Converter::EnemyMechenic(float dist, const sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime)
{
	auto newState = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	if (newState == EnemyState::Run && !is_attack)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		sf::Vector2f deltaMove = enemy_def.speed * deltaTime * dir;
		Move(nowMap, deltaMove);

		ChangeState(newState);
	}
	else if (newState == EnemyState::Spawn)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(newState);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		}
	}
	else if (newState == EnemyState::Stay)
	{
		ChangeState(newState);
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

Boss::Boss(const SpriteDef& spDef, const MapSprite& spMap, const EnemyDef& enemyDef, const ConverterDef& cDef, int id) :
	Enemy(spDef, spMap, enemyDef, id), converter_def{ cDef }
{
	now_spawn_count = (int)(cDef.maxSpawnCount * spMap.nowHealPoint / enemyDef.maxHealpoint);
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

void Boss::EnemyMechenic(float dist, const sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime)
{
	if (is_attack) { return; }
	auto newState = DetermineNewState(dist);

	float angle = map_sprite.angle * kPI / 180.0f;
	sf::Vector2f dir{ cos(angle), sin(angle) };

	if (newState == EnemyState::Run)
	{
		if (Random::BitRandom() > 0.7f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		sf::Vector2f deltaMove = enemy_def.speed * deltaTime * dir;
		Move(nowMap, deltaMove);

		ChangeState(newState);
	}
	else if (newState == EnemyState::Spawn || newState == EnemyState::Attack)
	{
		if (!CanChangeState()) return;

		if (is_can_attack)
		{
			ChangeState(newState);
		}
		else
		{
			if (Random::BitRandom() > 0.3f) map_sprite.angle = std::atan2(toPlayerDir.y, toPlayerDir.x) * 180.0f / kPI;
		}
	}
	else if (newState == EnemyState::Stay)
	{
		ChangeState(newState);
	}
}

void Boss::ChangeState(EnemyState newState)
{
	if (state == EnemyState::Dead) return;

	if (newState == EnemyState::Stay)
	{
		animr.SetAnimation(0);
	}
	else if (newState == EnemyState::Spawn)
	{
		animr.SetAnimation(0);
		is_attack = true;
		is_can_attack = false;
		now_time_attack = 0.0f;
	}
	else if (newState == EnemyState::Attack)
	{
		is_attack = true;
		is_can_attack = false;
		animr.SetAnimation(2);
		now_time_attack = 0.0f;
	}
	else if (newState == EnemyState::Run)
	{
		if (state != EnemyState::Run)
		{
			animr.SetAnimation(1, true);
		}
	}
	else if (newState == EnemyState::Dead)
	{
		Death();
	}

	state = newState;
}

EnemyState Boss::DetermineNewState(float dist)
{
	int hpPercent = (int)(abs(map_sprite.nowHealPoint - 1) / enemy_def.maxHealpoint * 100.0f);
	int a = hpPercent / 25;
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

Npc::Npc(const SpriteDef& _spDef, const MapSprite& _spMap, UIManager* _uiManager, Player* _player, const NpcDef& npcDef, int _id) :
	Sprite(_spDef, _spMap, _id), npc_def{ npcDef }, player{ _player }, ui_manager{ _uiManager }, now_key{ 1 }
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

void Npc::Update(int chooseKey)
{
	now_key = chooseKey;
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

FuncNpc::FuncNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef, ItemManager* _itemManager,
	UIManager* uiManager, Player* _player, int _id) :
	Npc(spDef, spMap, uiManager, _player, npcDef, _id), is_func{ false }, choose{ -1 }, item_manager{ _itemManager } {}

void FuncNpc::Stop()
{
	Npc::Stop();

	is_func = false;
	choose = -1;
}

void FuncNpc::Update(int chooseKey)
{
	now_key = chooseKey;

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

TradeNpc::TradeNpc(const SpriteDef& spDef, const MapSprite& spMap, const TraderDef& _tradeDef, const NpcDef& npcDef,
	ItemManager* _itemManager, UIManager* uiManager, Player* _player, int _id) :
	FuncNpc(spDef, spMap, npcDef, _itemManager, uiManager, _player, _id), trade_def{ _tradeDef } {}

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

	Itemble* res = item_manager->GetItemble(choose);

	if (res->cost > player->money) return;

	player->money -= res->cost;
	player->TakeItem(res);

	choose = -1;

	Init();
}

TravelerNpc::TravelerNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef,
	UIManager* uiManager, ItemManager* _itemManager, Player* _player, int _id) :
	FuncNpc(spDef, spMap, npcDef, _itemManager, uiManager, _player, _id) {}

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

		int maxSize = 1;
		auto& state = GameState::GetInstance();
		int levelN = state.data.levelNumber;
		if (state.data.killFirst &&
			state.data.killSecond &&
			state.data.killTherd)
		{
			maxSize = 5;
		}
		else
		{
			maxSize = std::min(levelN / 3, 4);
		}

		for (int i = 1; i < maxSize; i++)
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

ChangerNpc::ChangerNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef,
	UIManager* uiManager, ItemManager* itemManager, Player* _player, int _id) :
	FuncNpc(spDef, spMap, npcDef, itemManager, uiManager, _player, _id)
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
	auto& questM = QuestManager::GetInstance();
	questM.UpdateQuests(QuestType::CollectionMoney, 10 * coef);

	Init();
}

PortalNpc::PortalNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef,
	UIManager* uiManager, ItemManager* itemManager, Player* player, int _id) :
	FuncNpc(spDef, spMap, npcDef, itemManager, uiManager, player, _id) {}

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

MechanicNpc::MechanicNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef,
	UIManager* uiManager, ItemManager* itemManager, Player* player, int _id) :
	FuncNpc(spDef, spMap, npcDef, itemManager, uiManager, player, _id), type_upgade{ -1 } {}

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

	Gun* nowGun = player->guns[choose];

	if (player->money < 50 || player->details < 15 || nowGun->upgradeCount >= 5) return;

	player->money -= 50;
	player->details -= 15;

	Improve* imp = nullptr;
	if (type_upgade == 101)
	{
		imp = nowGun->DeleteImprove(ImproveType::Damage);
		nowGun->damage += 3;
	}
	else if (type_upgade == 102)
	{
		imp = nowGun->DeleteImprove(ImproveType::Magazin);
		nowGun->maxCount += 5;
	}
	else if (type_upgade == 103)
	{
		imp = nowGun->DeleteImprove(ImproveType::Spread);
		nowGun->maxRad += 3;
	}

	nowGun->TrySetImprove(imp);
	nowGun->upgradeCount++;

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

QuestNpc::QuestNpc(const SpriteDef& spDef, const MapSprite& spMap, const NpcDef& npcDef,
	UIManager* uiManager, ItemManager* itemManager, Player* player, int _id) :
	FuncNpc(spDef, spMap, npcDef, itemManager, uiManager, player, _id) {}

void QuestNpc::Init()
{
	if (!is_func)
	{
		Npc::Init();
	}
	else
	{
		ui_manager->DeleteNow();

		auto& questM = QuestManager::GetInstance();
		ui_manager->InitQuest((choose == -1 ? nullptr : questM.quests[choose - 1]), player);
	}
}

void QuestNpc::Use()
{
	if (choose == -1) return;

	auto& questM = QuestManager::GetInstance();
	player->money += questM.DeleteQuest(questM.quests[choose - 1]);

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
		QuestData data;
		data.progress = 0;
		data.type = QuestType(type);

		if (type == 0)
		{
			data.target = Random::IntRandom(5, 15);
			data.rewardCoins = Random::IntRandom(data.target * 10, data.target * 15);
		}
		else if (type == 1)
		{
			data.target = Random::IntRandom(100, 300);
			data.rewardCoins = Random::IntRandom((int)(data.target * 1.1f), (int)(data.target * 1.3f));
		}
		else if (type == 2)
		{
			data.target = Random::IntRandom(10, 25);
			data.rewardCoins = data.rewardCoins = Random::IntRandom(data.target * 10, data.target * 15);
		}

		auto& questM = QuestManager::GetInstance();
		questM.AddQuest(data);

		Init();
	}
	else if (now_key == 1 || now_key == 2 || now_key == 3)
	{
		choose = now_key;
		Init();
	}
}
