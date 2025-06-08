#ifndef WEAPON
#define WEAPON

#include "resources.h"
#include "sprite.h"
#include "animation.h"
#include "randomizer.h"
#include "sound_manager.h"
#include "event_system.h"
#include "const.h"
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

class Itemble {
public:
	Itemble(const std::wstring& name, const std::wstring& disc, int cost, int texture_id);
	virtual ~Itemble() = default;
	const std::wstring& GetName() const;
	const std::wstring& GetDiscription() const;
	const int GetCost() const;
	const int GetId() const;
protected:
	std::wstring name_;
	std::wstring disc_;
	int cost_;
	int id_;
};

class Item : public Itemble {
public:
	Item(const ItemsDef& item_def);
	Item() = default;
	void SetFunc(std::function<void(Player* player)>&& use_func);
	void UseItem(Player* sprite) const;
	const ItemType& GetType() const;
private:
	const ItemType type_;
	int max_using_;
	std::function<void(Player* sprite)> use_func_;
};

class Improve : public Itemble {
public:
	Improve(const ImproveDef& improve_def);
	Improve() = default;
	void SetGetFunc(std::function<void(Gun* gun)>&& set_effect);
	void SetDelFunc(std::function<void(Gun* gun)>&& del_effect);
	void TakeImprove(Gun* gun) const;
	void PutOffImprove(Gun* gun) const;
	const ImproveType& GetType() const;
private:
	const ImproveType type_;
	std::function<void(Gun* gun)> get_improve_;
	std::function<void(Gun* gun)> delete_improve_;
};

class Weapon {
public:
	Weapon(float time_betewen, float max_dist);
	Weapon() = default;
	virtual ~Weapon() = default;
	virtual void Update(float delta_time);
	virtual void DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta) const;
	virtual const bool IsCanUsed() const;
	virtual void SetAnimator(const Animator<sf::Texture*>&& anim);
	const float GetMaxDistance() const;
protected:
	virtual void StartAnimation(int number);
	virtual void Ussing(Enemy* enemy, float dist) = 0;

	Animator<sf::Texture*> weapon_animator_;
	float time_between_, now_time_;
	float max_dist_;
};

class Gun : public Weapon, public Itemble {
public:
	Gun(const GunDef& gun_def, bool is_reset, int dun_id);
	Gun() = default;
	Improve* const TrySetImprove(Improve* const improve);
	Improve* const DeleteImprove(ImproveType type);
	void Update(float delta_time) override;
	void UpdateRad(bool is_run, float delta_ime);
	const int ResetPatron(int count);
	const GunData GetGunData();
	void Ussing(Enemy* enemy, float dist) override;
	void ResetGunData();
	const bool IsResetable() const;
	void SetDamage(int damage);
	const int GetDamage() const;
	void SetMaxPatronsCount(int count);
	const int GetMaxPatronsCount() const;
	const int GetNowPatronsCount() const;
	void SetMaxRadius(int max_radius);
	const int GetMaxRadius() const;
	void SetMaxImpRadius(float max_imp_radius);
	const float GetMaxImpRadius() const;
	const float GetNowRadius() const;
	void TakeOneUpgrade();
	void SetUpgradeCount(int count);
	const int GetUpgradeCount() const;
	const std::map<ImproveType, Improve*>& GetAllImprovments() const;
private:
	float time_betwen_reset_, now_time_betwen_reset_;
	float now_rad_, max_imp_rad_;
	int max_rad_;
	int now_count_, max_count_;
	int damage_;
	int upgrade_count_;
	int gun_id_;
	bool is_reset_;
	std::map<ImproveType, Improve*> improvement_;
};

#endif // !WEAPON
