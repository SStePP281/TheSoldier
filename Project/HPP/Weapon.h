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
	Itemble(const std::wstring& name, const std::wstring& disc, int cost, int textureId);
	virtual ~Itemble() = default;
	std::wstring name;
	std::wstring disc;
	int cost;
	int id;
};

class Item : public Itemble
{
public:
	Item(const ItemsDef& def);
	Item() = default;
	void SetFunc(std::function<void(Player* player)>&& _useFunc);
	void UseItem(Player* sprite);
	ItemType type;
	int maxUsing;
	std::function<void(Player* sprite)> use_func;
};

class Improve : public Itemble
{
public:
	Improve(const ImproveDef& def);
	Improve() = default;
	void SetGetFunc(std::function<void(Gun* gun)>&& setEffect);
	void SetDelFunc(std::function<void(Gun* gun)>&& delEffect);
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
	virtual void Update(float dt);
	virtual void DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta);
	virtual bool IsCanUsed();
	virtual void SetAnimator(Animator<sf::Texture*>&& anim);

	float maxDist;
protected:
	virtual void StartAnimation(int number);
	virtual void Ussing(Enemy* sp, float dist) = 0;

	Animator<sf::Texture*> weaponAnimator;
	float timeBetwen, nowTime;
};

class Gun : public Weapon, public Itemble
{
public:
	Gun(const GunDef& def, bool isReset, int dunId);
	Gun() = default;

	Improve* TrySetImprove(Improve* improve);

	Improve* DeleteImprove(ImproveType type);

	void Update(float dt) override;

	void UpdateRad(bool isRun, float deltaTime);

	int ResetPatron(int count);

	GunData GetGunData();

	void Ussing(Enemy* sp, float dist) override;

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
