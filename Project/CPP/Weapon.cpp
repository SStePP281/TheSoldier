#include "Weapon.h"
#include "Player.h"

Weapon::Weapon(float _timeBetewen, float _maxDist)
	: timeBetwen{ _timeBetewen }, maxDist{ _maxDist } {
	nowTime = _timeBetewen;
}

void Weapon::update(float dt)
{
	weaponAnimator.update(dt);
	if (nowTime >= timeBetwen) return;
	nowTime += dt;
}

void Weapon::setAnimator(Animator<sf::Texture*>&& anim) { weaponAnimator = anim; }

void Weapon::drawWeapon(sf::RenderTarget* window, sf::Vector2f& delta)
{
	sf::Texture* tex = weaponAnimator.get();
	if (tex)
	{
		sf::Sprite weapon{ *tex };
		weapon.setOrigin(tex->getSize().x / 2.0f, (float)tex->getSize().y);
		weapon.scale(2.5f, 2.5f);
		weapon.setPosition(SCREEN_W / 2.0f - 20 + delta.x, SCREEN_H + delta.y + 10);
		window->draw(weapon);
	}
}

bool Weapon::isCanUsed()
{
	if (nowTime >= timeBetwen) return true;
	return false;
}

void Weapon::startAnimation(int number)
{
	nowTime = 0.0f;
	weaponAnimator.setAnimation(number);
}

Itemble::Itemble(std::wstring& _name, std::wstring& _disc, int _cost, int _textureId) :
	name{ _name }, disc{ _disc }, cost{ _cost }, id{ _textureId } {}

Improve::Improve(ImproveDef& def) : 
	Itemble(def.name, def.disc, def.cost, def.id), type{ def.type }
{
	if (type == Damage)
	{
		setGetFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage * def.effect);});

		setDelFunc([=](Gun* gun) {gun->damage = (int)round(gun->damage / def.effect);});
	}
	else if (type == Spread)
	{
		setGetFunc([=](Gun* gun) {gun->maxImpRad = round(gun->maxImpRad / def.effect);
		gun->nowRad = std::min(gun->maxImpRad, gun->nowRad);});

		setDelFunc([=](Gun* gun) {gun->maxImpRad = round(gun->maxImpRad * def.effect);
		gun->nowRad = std::min(gun->maxImpRad, gun->nowRad);});
	}
	else if (type == Magazin)
	{
		setGetFunc([=](Gun* gun) {gun->maxCount = (int)round(gun->maxCount * def.effect);
		gun->nowCount = std::min(gun->maxCount, gun->nowCount);});

		setDelFunc([=](Gun* gun) {gun->maxCount = (int)round(gun->maxCount / def.effect);
		gun->nowCount = std::min(gun->maxCount, gun->nowCount);});
	}
}

void Improve::setGetFunc(std::function<void(Gun* gun)>&& _setEffect) { getImprove = _setEffect; }

void Improve::setDelFunc(std::function<void(Gun* gun)>&& _delEffect) { deleteImprove = _delEffect; }

Item::Item(ItemsDef& def) : Itemble(def.name, def.disc, def.cost, def.id),
type{ def.type }, maxUsing{ def.maxUSing }
{
	if (def.type == Heal)
	{
		setFunc([=](Player* pl) {pl->enemy->spMap.nowHealPoint += def.effect;
		pl->enemy->spMap.nowHealPoint = std::min(pl->enemy->enemyDef.maxHealpoint, pl->enemy->spMap.nowHealPoint);});
	}
	else if (def.type == MaxHeal)
	{
		setFunc([=](Player* pl) {
			if (Random::bitRandom() > 0.2f)
				pl->enemy->enemyDef.maxHealpoint += def.effect;
			else
				pl->enemy->enemyDef.maxHealpoint -= def.effect / 2;
			pl->enemy->spMap.nowHealPoint = std::min(pl->enemy->enemyDef.maxHealpoint, pl->enemy->spMap.nowHealPoint);});
	}
	else if (def.type == MaxEnergy)
	{
		setFunc([=](Player* pl) {
			if (Random::bitRandom() > 0.2f)
				pl->maxEnergy += def.effect;
			else
				pl->maxEnergy -= def.effect / 2;
			pl->nowEnergy = std::min(pl->maxEnergy, pl->nowEnergy);});
	}
	else if (def.type == Armor)
	{
		setFunc([=](Player* pl) {pl->defence = (float)def.effect;
		pl->maxStrenght = (float)def.maxUSing;
		pl->nowStrenght = (float)def.maxUSing;});
	}
	else if (def.type == Patrons)
	{
		setFunc([=](Player* pl) {pl->patrons += def.effect;});
	}
}

void Item::setFunc(std::function<void(Player* sprite)>&& _useFunc) { useFunc = _useFunc; }

void Item::useItem(Player* sprite) { useFunc(sprite); }

Gun::Gun(GunDef& def, bool _isReset, int _gunId) : Weapon(def.shutTime, def.maxDist),
Itemble(def.name, def.disc, def.cost, def.id),
damage{ def.damage }, maxCount{ def.maxCount }, nowCount{ def.nowCount },
nowTimeBetwenReset{ def.resetTime }, timeBetwenReset{ def.resetTime }, 
isReset{ _isReset }, gunId{ _gunId }, upgradeCount{0}
{
	maxRad = MAX_RAD;
	nowRad = MIN_RAD;
	maxImpRad = MAX_RAD;
}

void Gun::update(float dt)
{
	Weapon::update(dt);
	if (isReset)
	{
		if (nowTimeBetwenReset >= timeBetwenReset) return;
		nowTimeBetwenReset += dt;
	}
}

void Gun::updateRad(bool isRun, float deltaTime)
{
	if (isRun)
	{
		nowRad = std::min(nowRad + MAX_RAD * deltaTime * 2, maxImpRad);
	}
	else
	{
		nowRad = std::max(nowRad - MAX_RAD * deltaTime * 2, 1.0f);
	}
}

int Gun::resetPatron(int count)
{
	if (!isReset) return count;
	
	if (nowTimeBetwenReset < timeBetwenReset && nowCount >= maxCount && !isCanUsed()) return count;
	
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
	SoundManager::playSound(Resources::gunsResetSound[gunId]);
	startAnimation(1);
	return count;
}

void Gun::ussing(Enemy* sp, float dist)
{
	if (nowCount == 0 && isReset)
	{
		SoundManager::playSound(Resources::gunCantShoutSound);
		return;
	}
	else if (isCanUsed() && (nowTimeBetwenReset >= timeBetwenReset || !isReset))
	{
		if (sp)
		{
			if (Random::bitRandom() > (nowRad - 0.05f) / MAX_RAD - 0.35f || nowRad == MIN_RAD)
			{
				sp->takeDamage((float)damage * (dist < maxDist ? 1 : 0));
			}
		}

		nowCount--;
		SoundManager::playSound(Resources::gunsShutSound[gunId]);
		startAnimation(0);
	}
}

Improve* Gun::trySetImprove(Improve* improve)
{
	if (!improve) return nullptr;

	auto temp = improvement[improve->type];
	if (temp) { temp->deleteImprove(this); }

	improvement[improve->type] = improve;
	improve->getImprove(this);

	return temp;
}

Improve* Gun::deleteImprove(ImproveType type)
{
	auto temp = improvement[type];
	if (temp != nullptr)
	{
		temp->deleteImprove(this);
		improvement.erase(type);
	}
	return temp;
}

GunData Gun::getGunData()
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
	imp = deleteImprove(Magazin);
	data.nowMaxCount = maxCount;
	trySetImprove(imp);

	imp = deleteImprove(Damage);
	data.nowDamage = damage;
	trySetImprove(imp);

	imp = deleteImprove(Spread);
	data.nowMaxRad = maxRad;
	trySetImprove(imp);

	return data;
}