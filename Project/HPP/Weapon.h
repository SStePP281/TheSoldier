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
	Itemble(const std::wstring& name, const std::wstring& disc, int cost, int texture_id);
	virtual ~Itemble() = default;

	std::wstring name;
	std::wstring disc;
	int cost;
	int id;
};

class Item : public Itemble
{
public:
	Item(const ItemsDef& item_def);
	Item() = default;
	void SetFunc(std::function<void(Player* player)>&& use_func);
	void UseItem(Player* sprite);

	ItemType type;
	int max_using;
	std::function<void(Player* sprite)> use_func;
};

class Improve : public Itemble
{
public:
	Improve(const ImproveDef& improve_def);
	Improve() = default;
	void SetGetFunc(std::function<void(Gun* gun)>&& set_effect);
	void SetDelFunc(std::function<void(Gun* gun)>&& del_effect);

	ImproveType type;
	std::function<void(Gun* gun)> get_improve;
	std::function<void(Gun* gun)> delete_improve;
};

class Weapon
{
public:
	Weapon(float time_betewen, float max_dist);
	Weapon() = default;
	virtual ~Weapon() = default;
	virtual void Update(float delta_time);
	virtual void DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta);
	virtual bool IsCanUsed();
	virtual void SetAnimator(Animator<sf::Texture*>&& anim);

	float max_dist;
protected:
	virtual void StartAnimation(int number);
	virtual void Ussing(Enemy* enemy, float dist) = 0;

	Animator<sf::Texture*> weapon_animator;
	float time_betwen, now_time;
};

class Gun : public Weapon, public Itemble
{
public:
	Gun(const GunDef& gun_def, bool is_reset, int dun_id);
	Gun() = default;

	Improve* TrySetImprove(Improve* improve);

	Improve* DeleteImprove(ImproveType type);

	void Update(float delta_time) override;

	void UpdateRad(bool is_run, float delta_ime);

	int ResetPatron(int count);

	GunData GetGunData();

	void Ussing(Enemy* sp, float dist) override;

	bool is_reset;
	int now_count;
	int max_count;
	int damage;
	int upgrade_count;
	float now_rad, max_imp_rad;
	int max_rad;
	std::map<ImproveType, Improve*> improvement;
private:
	int gun_id;
	float time_betwen_reset, now_time_betwen_reset;
};

#endif // !WEAPON
