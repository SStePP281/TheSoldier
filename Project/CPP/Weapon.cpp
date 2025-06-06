#include "Weapon.h"
#include "Player.h"

Weapon::Weapon(float time_betewen, float max_dist) : time_betwen{ time_betewen }, 
now_time{ time_betewen }, max_dist{ max_dist } {}

void Weapon::Update(float delta_time)
{
	weapon_animator.Update(delta_time);
	if (now_time >= time_betwen) return;
	now_time += delta_time;
}

void Weapon::SetAnimator(Animator<sf::Texture*>&& animator) { weapon_animator = animator; }

void Weapon::DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta_shake)
{
	sf::Texture* texture = weapon_animator.Get();

	if (texture)
	{
		sf::Sprite weapon{ *texture };
		weapon.setOrigin(texture->getSize().x / 2.0f, (float)texture->getSize().y);
		weapon.scale(2.5f, 2.5f);
		weapon.setPosition(kScreenWight / 2.0f - 20 + delta_shake.x, kScreenHeight + delta_shake.y + 10);
		window->draw(weapon);
	}
}

bool Weapon::IsCanUsed()
{
	return now_time >= time_betwen;
}

void Weapon::StartAnimation(int number)
{
	now_time = 0.0f;
	weapon_animator.SetAnimation(number);
}

Itemble::Itemble(const std::wstring& name, const std::wstring& disc, int cost, int texture_id) :
	name{ name }, disc{ disc }, cost{ cost }, id{ texture_id } {}

Improve::Improve(const ImproveDef& improve_def) :
	Itemble(improve_def.name, improve_def.disc, improve_def.cost, improve_def.id), type{ improve_def.type }
{
	if (type == ImproveType::Damage)
	{
		SetGetFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage * improve_def.effect);});

		SetDelFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage / improve_def.effect);});
	}
	else if (type == ImproveType::Spread)
	{
		SetGetFunc([=](Gun* gun) {gun->max_imp_rad = round(gun->max_imp_rad / improve_def.effect);
		gun->now_rad = std::min(gun->max_imp_rad, gun->now_rad);});

		SetDelFunc([=](Gun* gun) {gun->max_imp_rad = round(gun->max_imp_rad * improve_def.effect);
		gun->now_rad = std::min(gun->max_imp_rad, gun->now_rad);});
	}
	else if (type == ImproveType::Magazin)
	{
		SetGetFunc([=](Gun* gun) {gun->max_count = (int)round(gun->max_count * improve_def.effect);
		gun->now_count = std::min(gun->max_count, gun->now_count);});

		SetDelFunc([=](Gun* gun) {gun->max_count = (int)round(gun->max_count / improve_def.effect);
		gun->now_count = std::min(gun->max_count, gun->now_count);});
	}
}

void Improve::SetGetFunc(std::function<void(Gun* gun)>&& set_effect) { get_improve = set_effect; }

void Improve::SetDelFunc(std::function<void(Gun* gun)>&& del_effect) { delete_improve = del_effect; }

Item::Item(const ItemsDef& item_def) : Itemble(item_def.name, item_def.disc, item_def.cost, item_def.id),
type{ item_def.type }, max_using{ item_def.maxUSing }
{
	if (item_def.type == ItemType::Heal)
	{
		SetFunc([=](Player* pl) {pl->enemy->map_sprite.nowHealPoint += item_def.effect;
		pl->enemy->map_sprite.nowHealPoint = std::min(pl->enemy->enemy_def.maxHealpoint, pl->enemy->map_sprite.nowHealPoint);});
	}
	else if (item_def.type == ItemType::MaxHeal)
	{
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f)
				pl->enemy->enemy_def.maxHealpoint += item_def.effect;
			else
				pl->enemy->enemy_def.maxHealpoint -= item_def.effect / 2;
			pl->enemy->map_sprite.nowHealPoint = std::min(pl->enemy->enemy_def.maxHealpoint, pl->enemy->map_sprite.nowHealPoint);});
	}
	else if (item_def.type == ItemType::MaxEnergy)
	{
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f)
				pl->max_energy += item_def.effect;
			else
				pl->max_energy -= item_def.effect / 2;
			pl->now_energy = std::min(pl->max_energy, pl->now_energy);});
	}
	else if (item_def.type == ItemType::Armor)
	{
		SetFunc([=](Player* pl) {pl->defence = (float)item_def.effect;
		pl->max_strenght = (float)item_def.maxUSing;
		pl->now_strenght = (float)item_def.maxUSing;});
	}
	else if (item_def.type == ItemType::Patrons)
	{
		SetFunc([=](Player* pl) {pl->patrons += item_def.effect;});
	}
}

void Item::SetFunc(std::function<void(Player* sprite)>&& _use_func) { use_func = _use_func; }

void Item::UseItem(Player* sprite) 
{ 
	use_func(sprite); 
}

Gun::Gun(const GunDef& gun_def, bool is_reset, int gun_id) : Weapon(gun_def.shutTime, gun_def.maxDist),
Itemble(gun_def.name, gun_def.disc, gun_def.cost, gun_def.id), damage{ gun_def.damage }, upgrade_count{ 0 },
max_count{ gun_def.maxCount }, now_count{ gun_def.nowCount },now_time_betwen_reset{ gun_def.resetTime }, 
time_betwen_reset{ gun_def.resetTime }, is_reset{ is_reset }, gun_id{ gun_id }, max_rad{kMaxRad}, now_rad{kMinRad},
max_imp_rad{kMaxRad} {}

void Gun::Update(float delta_time)
{
	Weapon::Update(delta_time);

	if (is_reset)
	{
		if (now_time_betwen_reset >= time_betwen_reset) return;
		now_time_betwen_reset += delta_time;
	}
}

void Gun::UpdateRad(bool is_run, float delta_time)
{
	if (is_run)
	{
		now_rad = std::min(now_rad + kMaxRad * delta_time * 2, max_imp_rad);
	}
	else
	{
		now_rad = std::max(now_rad - kMaxRad * delta_time * 2, 1.0f);
	}
}

int Gun::ResetPatron(int count)
{
	if (!is_reset) return count;
	
	if (now_time_betwen_reset < time_betwen_reset && now_count >= max_count && !IsCanUsed()) return count;
	
	auto delta = max_count - now_count;

	if (delta <= 0 || count == 0)
	{
		return count;
	}
	else if (delta < count)
	{
		count -= delta;
		now_count = max_count;
	}
	else if (count > 0)
	{
		now_count += count;
		count = 0;
	}

	now_time_betwen_reset = 0;
	SoundManager::PlaySounds(Resources::guns_reset_sound[gun_id]);
	StartAnimation(1);
	return count;
}

void Gun::Ussing(Enemy* enemy, float dist)
{
	if (now_count == 0 && is_reset)
	{
		SoundManager::PlaySounds(Resources::gun_cant_shout_sound);
		return;
	}
	else if (IsCanUsed() && (now_time_betwen_reset >= time_betwen_reset || !is_reset))
	{
		if (enemy)
		{
			if (Random::BitRandom() > (now_rad - 0.05f) / kMaxRad - 0.35f || now_rad == kMinRad)
			{
				enemy->TakeDamage((float)damage * (dist < max_dist ? 1 : 0));
			}
		}

		now_count--;
		SoundManager::PlaySounds(Resources::guns_shut_sound[gun_id]);
		StartAnimation(0);
	}
}

Improve* Gun::TrySetImprove(Improve* improve)
{
	if (!improve) return nullptr;

	auto temp = improvement[improve->type];
	if (temp) { temp->delete_improve(this); }

	improvement[improve->type] = improve;
	improve->get_improve(this);

	return temp;
}

Improve* Gun::DeleteImprove(ImproveType type)
{
	auto temp = improvement[type];
	if (temp != nullptr)
	{
		temp->delete_improve(this);
		improvement.erase(type);
	}
	return temp;
}

GunData Gun::GetGunData()
{
	GunData data;
	for (auto im : improvement)
	{
		if (im.second)
		{
			data.improveId.push_back(im.second->id);
		}
	}
	data.id = id;
	data.upgradeCount = upgrade_count;
	data.nowCount = now_count;

	Improve* imp;
	imp = DeleteImprove(ImproveType::Magazin);
	data.nowMaxCount = max_count;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Damage);
	data.nowDamage = damage;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Spread);
	data.nowMaxRad = max_rad;
	TrySetImprove(imp);

	return data;
}