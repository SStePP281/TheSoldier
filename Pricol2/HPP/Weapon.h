#pragma once
#ifndef WEAPON
#define WEAPON

#include "Resources.h"
#include "Sprite.h"
#include "Animation.h"
#include "Randomizer.h"
#include "SoundManager.h"
#include "EventSystem.h"
#include "CONST.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <array>
#include <functional>

class Gun;
class Player;

class Itemble
{
public:
	Itemble(std::wstring& name, std::wstring& disc, int cost, int textureId);
	virtual ~Itemble() = default;
	std::wstring name;
	std::wstring disc;
	int cost;
	int id;
};

class Item : public Itemble
{
public:
	Item(ItemsDef& def);
	Item() = default;
	void setFunc(std::function<void(Player* player)>&& _useFunc);
	void useItem(Player* sprite);
	ItemType type;
	int maxUsing;
	std::function<void(Player* sprite)> useFunc;
};

class Improve : public Itemble
{
public:
	Improve(ImproveDef& def);
	Improve() = default;
	void setGetFunc(std::function<void(Gun* gun)>&& setEffect);
	void setDelFunc(std::function<void(Gun* gun)>&& delEffect);
	ImproveType type;
	std::function<void(Gun* gun)> getImprove;
	std::function<void(Gun* gun)> deleteImprove;
};

class Weapon
{
public:
	Weapon(float _timeBetewen, float maxDist);
	Weapon() = default;
	virtual ~Weapon() = default;
	virtual void update(float dt);
	virtual void drawWeapon(sf::RenderTarget* window, sf::Vector2f& delta);
	virtual bool isCanUsed();
	virtual void setAnimator(Animator<sf::Texture*>&& anim);

	float maxDist;
protected:
	virtual void startAnimation(int number);
	virtual void ussing(Enemy* sp, float dist) = 0;

	Animator<sf::Texture*> weaponAnimator;
	float timeBetwen, nowTime;
};

class Gun : public Weapon, public Itemble
{
public:
	Gun(GunDef& def, bool isReset, int dunId);
	Gun() = default;

	Improve* trySetImprove(Improve* improve);

	Improve* deleteImprove(ImproveType type);

	void update(float dt) override;

	void updateRad(bool isRun, float deltaTime);

	int resetPatron(int count);

	GunData getGunData();

	void ussing(Enemy* sp, float dist) override;

	bool isReset;
	int nowCount;
	int maxCount;
	int damage;
	int upgradeCount;
	float nowRad, maxImpRad;
	int maxRad;
	std::map<ImproveType, Improve*> improvement;
private:
	int gunId;
	float timeBetwenReset, nowTimeBetwenReset;
};

#endif // !WEAPON