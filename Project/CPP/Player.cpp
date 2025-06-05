#include "Player.h"
#include "Inventory.h"

Player::Player(Enemy* sprite, const PlayerDef& def, Map* now_map) : 
	now_energy{ def.nowEnergy }, max_energy { def.maxEnergy },
	defence{ def.defence}, now_strenght{ def.nowStrenght }, max_strenght{ def.maxStrenght }, 
	patrons{ def.countpantrons }, money{ def.money }, details{ def.details },
	now_map{ now_map }, enemy{ sprite }, now_gun{ 0 }, now_heal{ nullptr }, kick{ nullptr },
	inventory{ nullptr }, pitch{ 0 }, shake_time{ 0 }, pos_z{ 0 }, is_jump{ false }, jump_flag{ false },
	boost_speed{ 8.0f }, now_speed{ enemy->enemy_def.speed }, mouse_speed{1.0f}
{
	enemy->texture_size = 0;

	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int NON) { auto& data = Data::GetInstance(); data.SavePlayerData(this);});

	event.Subscribe<int>("RESET_GAME", [=](const int NON) { ResetPlayer(); });

	event.Subscribe<int>("WIN_GAME", [=](const int NON) { ResetPlayer(); });
}

void Player::ResetPlayer()
{
	now_energy = 2.0f;
	max_energy = 2.0f;
	defence = 0.0f;
	now_strenght = 0.0f;
	max_strenght = 0.0f;
	patrons = 200;
	money = 500;
	details = 0;
	now_heal = nullptr;
	enemy->enemy_def.maxHealpoint = 100.0f;
	enemy->map_sprite.nowHealPoint = 100.0f;
}

Gun* Player::SetGun(Gun* gun, int position) 
{
	auto temp = guns[position];
	guns[position] = gun;
	return temp;
}

Gun* Player::GetGun(int position) { return guns[position]; }

void Player::SetInventory(Inventory* _inventory) 
{ 
	if (inventory = _inventory; inventory) { now_heal = inventory->TakeMaxHeal(); }
}

void Player::UpdateMouseData(const sf::Vector2f& mouse_position, float delta_time)
{
	enemy->map_sprite.angle += mouse_speed * kRotationSpeed * mouse_position.x * delta_time;

	pitch = std::clamp(pitch - mouse_position.y * delta_time * kVerticalMouseSpeed, -200.0f, 200.0f);

	guns[now_gun]->Update(delta_time);
	if (kick) kick->Update(delta_time);

	Gravity(delta_time);
}

void Player::CheckBoost(bool is_pressed, float delta_time)
{
	static bool boost_flag = false;

	now_speed = enemy->enemy_def.speed;

	if (boost_flag) {
		now_energy = std::min(now_energy + delta_time, max_energy);
		if (now_energy >= max_energy) {
			boost_flag = false;
		}
		return;
	}

	if (!is_pressed) {
		now_energy = std::min(now_energy + delta_time, max_energy);
	}
	else {
		if (now_energy > 0) {
			now_speed = boost_speed;
			now_energy = std::max(now_energy - delta_time * 1.2f, 0.0f);
		}
		else {
			boost_flag = true;
		}
	}
}

void Player::Move(const sf::Vector2f& delta_position, float delta_time)
{
	guns[now_gun]->UpdateRad(delta_position != sf::Vector2f(), delta_time);
	sf::Vector2f delta_move = delta_position * delta_time * now_speed;
	enemy->Move(now_map, delta_move);
	RhakeCamera(delta_time, delta_position != sf::Vector2f());
}

void Player::RhakeCamera(float delta_time, bool is_run)
{
	if (is_run)
	{
		float scale = 5.0f;
		shake_time += delta_time;
		shake_delta.x = cosf(shake_time * now_speed) * scale;
		shake_delta.y = sinf(2 * shake_time * now_speed) * scale;
	}
	else
	{
		shake_time = 0;
		if (shake_delta == sf::Vector2f()) return;

		shake_delta *= 0.9f;

		shake_delta.x = std::abs(shake_delta.x) < 0.0001f ? 0.0f : shake_delta.x;
		shake_delta.y = std::abs(shake_delta.y) < 0.0001f ? 0.0f : shake_delta.y;
	}
}

void Player::Gravity(float delta_time)
{
	if (!is_jump) return;
	
	if (pos_z < 0)
	{
		is_jump = false;
		jump_flag = false;
		pos_z = 0;
	}
	else
	{
		if (pos_z > 150) jump_flag = true;

		pos_z += (jump_flag ? -750 : 750) * delta_time;
		pos_z = std::max(pos_z, -1.0f);
	}
}

void Player::Jump() { if (pos_z == 0) { is_jump = true; } }

void Player::ReloadingGun() { patrons = guns[now_gun]->ResetPatron(patrons); }

void Player::TakeDamage(float damage)
{
	if (now_strenght > 0)
	{
		damage *= (1 - defence / 100.f);
		now_strenght -= damage * 10.0f / defence;

		if (now_strenght <= 0)
		{
			now_strenght = 0;
			max_strenght = 0;
			defence = 0;
		}
	}

	enemy->TakeDamage(damage);
}

void Player::Fire(int gun)
{
	float radians_angle = enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f direction(cos(radians_angle), sin(radians_angle));

	if (gun == -1)
	{
		if (kick->IsCanUsed() && guns[now_gun]->IsCanUsed())
		{
			RayHit hit = Raycast(now_map, enemy->map_sprite.position, direction, false, enemy, 1);
			if (hit.cell == 1) now_map->SetNewOnGrid(hit.mapPos.x, hit.mapPos.y, kWallLayer, 0);
			kick->Ussing(nullptr, 0);
		}
	}
	else
	{
		if (guns[now_gun]->IsCanUsed() && kick->IsCanUsed())
		{
			RayHit hit = Raycast(now_map, enemy->map_sprite.position, direction, true, enemy, guns[now_gun]->maxDist, pitch);
			float dist = hit.sprite && hit.sprite->sprite_def.type != SpriteType::NPC ? dist = sqrt(GETDIST(hit.sprite->map_sprite.position, enemy->map_sprite.position)): 0;
			guns[now_gun]->Ussing(dynamic_cast<Enemy*>(hit.sprite), dist);
		}
	}
}

Sprite* Player::Dialog()
{
	float radians_angle = enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f direction(cos(radians_angle), sin(radians_angle));

	RayHit hit = Raycast(now_map, enemy->map_sprite.position, direction, true, enemy, 1, pitch);
	return hit.sprite && hit.sprite->sprite_def.type == SpriteType::NPC ? hit.sprite : nullptr;
}

void Player::SwapGun(bool flag)
{
	int delta = flag ? 1 : -1;
	now_gun = (now_gun + delta + 3) % 3;

	while (!guns[now_gun]) 
	{
		now_gun = (now_gun + delta + 3) % 3;
	}
}

PlayerDef Player::GetPlayerDef()
{
	std::vector<int> guns_data;

	for (auto it : guns)
	{
		if (it != nullptr && it->name != L"Кулак")
		{
			guns_data.push_back(it->id);
		}
	}

	return { enemy->enemy_def.maxHealpoint,
	enemy->map_sprite.nowHealPoint,
	max_energy,
	now_energy,
	defence,
	max_strenght,
	now_strenght,
	patrons,
	money,
	details,
	guns_data };
}

void Player::SetNemMap(Map* _map) { now_map = _map; }

Gun* Player::GetNowGun() { return guns[now_gun]; }

float Player::GetMoveSpeed() { return now_speed; }

void Player::TakeItem(Itemble* item, int count)
{
	inventory->TakeItem(item, count);
	now_heal = inventory->TakeMaxHeal();
}

void Player::Heal()
{
	if (now_heal)
	{
		now_heal->use_func(this);
		inventory->UseItem(now_heal);
		now_heal = inventory->TakeMaxHeal();
	}
}