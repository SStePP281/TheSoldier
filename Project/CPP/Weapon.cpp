#include "Weapon.h"
#include "Player.h"

Weapon::Weapon(float _timeBetewen, float _maxDist)
	: timeBetwen{ _timeBetewen }, maxDist{ _maxDist } {
	nowTime = _timeBetewen;
}

void Weapon::Update(float dt)
{
	weaponAnimator.Update(dt);
	if (nowTime >= timeBetwen) return;
	nowTime += dt;
}

void Weapon::SetAnimator(Animator<sf::Texture*>&& anim) { weaponAnimator = anim; }

void Weapon::DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta)
{
	sf::Texture* tex = weaponAnimator.Get();
	if (tex)
	{
		sf::Sprite weapon{ *tex };
		weapon.setOrigin(tex->getSize().x / 2.0f, (float)tex->getSize().y);
		weapon.scale(2.5f, 2.5f);
		weapon.setPosition(kScreenWight / 2.0f - 20 + delta.x, kScreenHeight + delta.y + 10);
		window->draw(weapon);
	}
}

bool Weapon::IsCanUsed()
{
	if (nowTime >= timeBetwen) return true;
	return false;
}

void Weapon::StartAnimation(int number)
{
	nowTime = 0.0f;
	weaponAnimator.SetAnimation(number);
}

Itemble::Itemble(const std::wstring& _name, const std::wstring& _disc, int _cost, int _textureId) :
	name{ _name }, disc{ _disc }, cost{ _cost }, id{ _textureId } {}

Improve::Improve(const ImproveDef& def) :
	Itemble(def.name, def.disc, def.cost, def.id), type{ def.type }
{
	if (type == ImproveType::Damage)
	{
		SetGetFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage * def.effect);});

		SetDelFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage / def.effect);});
	}
	else if (type == ImproveType::Spread)
	{
		SetGetFunc([=](Gun* gun) {gun->maxImpRad = round(gun->maxImpRad / def.effect);
		gun->nowRad = std::min(gun->maxImpRad, gun->nowRad);});

		SetDelFunc([=](Gun* gun) {gun->maxImpRad = round(gun->maxImpRad * def.effect);
		gun->nowRad = std::min(gun->maxImpRad, gun->nowRad);});
	}
	else if (type == ImproveType::Magazin)
	{
		SetGetFunc([=](Gun* gun) {gun->maxCount = (int)round(gun->maxCount * def.effect);
		gun->nowCount = std::min(gun->maxCount, gun->nowCount);});

		SetDelFunc([=](Gun* gun) {gun->maxCount = (int)round(gun->maxCount / def.effect);
		gun->nowCount = std::min(gun->maxCount, gun->nowCount);});
	}
}

void Improve::SetGetFunc(std::function<void(Gun* gun)>&& _setEffect) { getImprove = _setEffect; }

void Improve::SetDelFunc(std::function<void(Gun* gun)>&& _delEffect) { deleteImprove = _delEffect; }

Item::Item(const ItemsDef& def) : Itemble(def.name, def.disc, def.cost, def.id),
type{ def.type }, maxUsing{ def.maxUSing }
{
	if (def.type == ItemType::Heal)
	{
		SetFunc([=](Player* pl) {pl->enemy->map_sprite.nowHealPoint += def.effect;
		pl->enemy->map_sprite.nowHealPoint = std::min(pl->enemy->enemy_def.maxHealpoint, pl->enemy->map_sprite.nowHealPoint);});
	}
	else if (def.type == ItemType::MaxHeal)
	{
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f)
				pl->enemy->enemy_def.maxHealpoint += def.effect;
			else
				pl->enemy->enemy_def.maxHealpoint -= def.effect / 2;
			pl->enemy->map_sprite.nowHealPoint = std::min(pl->enemy->enemy_def.maxHealpoint, pl->enemy->map_sprite.nowHealPoint);});
	}
	else if (def.type == ItemType::MaxEnergy)
	{
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f)
				pl->max_energy += def.effect;
			else
				pl->max_energy -= def.effect / 2;
			pl->now_energy = std::min(pl->max_energy, pl->now_energy);});
	}
	else if (def.type == ItemType::Armor)
	{
		SetFunc([=](Player* pl) {pl->defence = (float)def.effect;
		pl->max_strenght = (float)def.maxUSing;
		pl->now_strenght = (float)def.maxUSing;});
	}
	else if (def.type == ItemType::Patrons)
	{
		SetFunc([=](Player* pl) {pl->patrons += def.effect;});
	}
}

void Item::SetFunc(std::function<void(Player* sprite)>&& _useFunc) { use_func = _useFunc; }

void Item::UseItem(Player* sprite) { use_func(sprite); }

Gun::Gun(const GunDef& def, bool _isReset, int _gunId) : Weapon(def.shutTime, def.maxDist),
Itemble(def.name, def.disc, def.cost, def.id),
damage{ def.damage }, maxCount{ def.maxCount }, nowCount{ def.nowCount },
nowTimeBetwenReset{ def.resetTime }, timeBetwenReset{ def.resetTime }, 
isReset{ _isReset }, gunId{ _gunId }, upgradeCount{0}
{
	maxRad = kMaxRad;
	nowRad = kMinRad;
	maxImpRad = kMaxRad;
}

void Gun::Update(float dt)
{
	Weapon::Update(dt);
	if (isReset)
	{
		if (nowTimeBetwenReset >= timeBetwenReset) return;
		nowTimeBetwenReset += dt;
	}
}

void Gun::UpdateRad(bool isRun, float deltaTime)
{
	if (isRun)
	{
		nowRad = std::min(nowRad + kMaxRad * deltaTime * 2, maxImpRad);
	}
	else
	{
		nowRad = std::max(nowRad - kMaxRad * deltaTime * 2, 1.0f);
	}
}

int Gun::ResetPatron(int count)
{
	if (!isReset) return count;
	
	if (nowTimeBetwenReset < timeBetwenReset && nowCount >= maxCount && !IsCanUsed()) return count;
	
	auto delta = maxCount - nowCount;

	if (delta <= 0 || count == 0)
	{
		return count;
	}
	else if (delta < count)
	{
		count -= delta;
		nowCount = maxCount;
	}
	else if (count > 0)
	{
		nowCount += count;
		count = 0;
	}

	nowTimeBetwenReset = 0;
	SoundManager::PlaySounds(Resources::guns_reset_sound[gunId]);
	StartAnimation(1);
	return count;
}

void Gun::Ussing(Enemy* sp, float dist)
{
	if (nowCount == 0 && isReset)
	{
		SoundManager::PlaySounds(Resources::gun_cant_shout_sound);
		return;
	}
	else if (IsCanUsed() && (nowTimeBetwenReset >= timeBetwenReset || !isReset))
	{
		if (sp)
		{
			if (Random::BitRandom() > (nowRad - 0.05f) / kMaxRad - 0.35f || nowRad == kMinRad)
			{
				sp->TakeDamage((float)damage * (dist < maxDist ? 1 : 0));
			}
		}

		nowCount--;
		SoundManager::PlaySounds(Resources::guns_shut_sound[gunId]);
		StartAnimation(0);
	}
}

Improve* Gun::TrySetImprove(Improve* improve)
{
	if (!improve) return nullptr;

	auto temp = improvement[improve->type];
	if (temp) { temp->deleteImprove(this); }

	improvement[improve->type] = improve;
	improve->getImprove(this);

	return temp;
}

Improve* Gun::DeleteImprove(ImproveType type)
{
	auto temp = improvement[type];
	if (temp != nullptr)
	{
		temp->deleteImprove(this);
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
	data.upgradeCount = upgradeCount;
	data.nowCount = nowCount;

	Improve* imp;
	imp = DeleteImprove(ImproveType::Magazin);
	data.nowMaxCount = maxCount;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Damage);
	data.nowDamage = damage;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Spread);
	data.nowMaxRad = maxRad;
	TrySetImprove(imp);

	return data;
}