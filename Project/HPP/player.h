#ifndef PLAYER
#define PLAYER

#include "map.h"
#include "resources.h"
#include "animation.h"
#include "weapon.h"
#include "raycast.h"
#include "const.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

class Inventory;

class Player {
public:
	Player(Enemy* const sprite, const PlayerDef& def, Map* nowMap);
	~Player() = default;
	void SetInventory(Inventory* const inventory);
	void UpdateMouseData(const sf::Vector2f& mouse_position, float delta_time);
	void CheckBoost(bool is_pressed, float delta_time);
	void Move(const sf::Vector2f& delta_position, float delta_time);
	void Jump();
	const Sprite* const Dialog() const;
	void TakeDamage(float damage);
	void Fire(int gun = -1) const;
	void SwapGun(bool flag);
	void ReloadingGun();
	void TakeItem(Itemble* const itemble, int cnt = 1);
	void Heal();
	const Gun* const SetGun(Gun* const gun, int position);
	Gun* const GetGun(int pos) const;
	const float GetMoveSpeed() const;
	const Gun* const GetNowGun() const;
	const Gun* const GetKick() const;
	void SetKick(Gun* const gun);
	const PlayerDef GetPlayerDef() const;
	void SetNemMap(Map* map);
	void ResetPlayer(const PlayerDef player_def);
	const int GetMoney() const;
	void SetMoney(int new_money);
	const int GetDetails() const;
	void SetDetails(int new_details);
	Enemy* const GetEnemy() const;
	const float GetMaxEnergy() const;
	const float GetNowEnergy() const;
	void SetMaxEnergy(float value);
	const float GetMaxStrenght() const;
	const float GetNowStrenght() const;
	void SetMaxStrenght(float value);
	void SetDefence(float value);
	void SetPatrons(int new_count);
	const int GetPatronsCount() const;
	const Item* const GetNowHeal() const;
	void SetNowHeal(Item* const heal);
	void SetMouseSpeed(float value);
	const int GetPositionZ() const;
	const int GetPitch() const;
	const sf::Vector2f& GetShakeDelta() const;
private:
	void ResetPlayer();
	void RhakeCamera(float delta_time, bool is_run);
	void Gravity(float delta_time);

	Enemy* const enemy_;
	Gun* kick_;
	Inventory* inventory_;
	Map* now_map_;
	Item* now_heal_;
	Gun* guns_[3]{};
	bool is_jump_, jump_flag_;
	float now_speed_, boost_speed_, shake_time_;
	float pitch_, pos_z_, max_energy_, now_energy_;
	float defence_, now_strenght_, max_strenght_;
	float mouse_speed_;
	int patrons_;
	int money_;
	int details_;
	int now_gun_;
	sf::Vector2f shake_delta_;
};

#endif // !PLAYER
