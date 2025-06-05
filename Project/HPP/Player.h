#ifndef PLAYER
#define PLAYER

#include "Map.h"
#include "Resources.h"
#include "Animation.h"
#include "Weapon.h"
#include "Raycast.h"
#include "CONST.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

class Inventory;

class Player
{
public:
	Player(Enemy* sprite, const PlayerDef& def, Map* nowMap);
	~Player() = default;
	void SetInventory(Inventory* inventory);
	void UpdateMouseData(const sf::Vector2f& mouse_position, float delta_time);
	void CheckBoost(bool is_pressed, float delta_time);
	void Move(const sf::Vector2f& delta_position, float delta_time);
	void Jump();
	Sprite* Dialog();
	void TakeDamage(float damage);
	void Fire(int gun = -1);
	void SwapGun(bool flag);
	void ReloadingGun();
	Gun* SetGun(Gun* gun, int position);
	Gun* GetGun(int pos);
	float GetMoveSpeed();
	void TakeItem(Itemble*, int cnt = 1);
	void Heal();

	Gun* GetNowGun();
	PlayerDef GetPlayerDef();
	void SetNemMap(Map* map);

	Enemy* enemy;
	Gun* kick;
	float pitch, pos_z, max_energy, now_energy;
	float defence, now_strenght, max_strenght;
	float mouse_speed;
	int patrons;
	int money;
	int details;
	Item* now_heal;
	Gun* guns[3]{};
	sf::Vector2f shake_delta;
private:
	void ResetPlayer();
	void RhakeCamera(float delta_time, bool is_run);
	void Gravity(float delta_time);

	Inventory* inventory;
	bool is_jump, jump_flag;
	Map* now_map;
	int now_gun;
	float now_speed, boost_speed, shake_time;
};

#endif // !PLAYER
