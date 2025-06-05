#ifndef SPRITE
#define SPRITE

#include "CONST.h"
#include "Animation.h"
#include "Quest.h"
#include <SFML/Graphics/Texture.hpp>
#include <functional>
#include <algorithm>
#include <memory>
#include <set>

class Map;
class Gun;
class Player;
class UIManager;
class ItemManager;

class Sprite
{
public:
	Sprite(const SpriteDef& sprite_def, const MapSprite& map_sprite, int id);
	Sprite() = default;
	virtual ~Sprite() = default;
	virtual std::pair<int, bool> GetTextIndex();

	int id;
	int texture_size;
	SpriteDef sprite_def;
	MapSprite map_sprite;
	sf::Texture* texture;
	std::set<std::tuple<int, int>> blockmap_coords;
};

class Enemy : public Sprite
{
public:
	Enemy(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, int id);
	virtual ~Enemy() = default;
	virtual std::pair<int, bool> GetTextIndex() override;
	virtual void Death();
	virtual void Attack(Player* player);
	void Update(float delta_time);
	void Move(Map* map, const sf::Vector2f& move);
	virtual void TakeDamage(float damage);
	virtual EnemyState DetermineNewState(float dist);
	virtual void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time);
	virtual bool CanChangeState();
	virtual void ChangeState(EnemyState new_state);

	EnemyDef enemy_def;
	bool is_can_attack = false;
	bool is_attack = false;
	Animator<int> animr;
	float time_atecked, now_time_attack;
protected:
	void UpdateTimeSinceLastAttack(float delta_time);
	void UpdateTimeSinceDamaged(float delta_time);
	bool CheckCollision(Map* map, const sf::Vector2f& new_pos, bool x_axis);

	bool is_damaged;
	EnemyState state;
};

class Converter : public Enemy
{
public:
	Converter(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& converter_def, int id);
	void TakeDamage(float damege) override;
	void Death() override;
	void Attack(Player* plaer) override;
	void ChangeState(EnemyState new_state) override;
	bool CanChangeState() override;
	void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time) override;
	EnemyState DetermineNewState(float dist) override;

	ConverterDef converter_def;
private:
	int now_spawn_count;
};

class Boss : public Enemy
{
public:
	Boss(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& converter_def, int id);
	void Death() override;
	void Attack(Player* plaer) override;
	void ChangeState(EnemyState new_state) override;
	void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* now_map, float delta_time) override;
	EnemyState DetermineNewState(float dist) override;

	ConverterDef converter_def;
private:
	int now_spawn_count;
};

class Npc : public Sprite
{
public:
	Npc(const SpriteDef& sprite_def, const MapSprite& map_sprite, UIManager* ui_manager, Player* player, const NpcDef& npc_def, int id);
	Npc() = default;
	virtual ~Npc() = default;
	void SetEndFunc(std::function<void()>&& endFunc);
	virtual void Init();
	virtual void Stop();
	virtual void Use();
	virtual void Update(int choose_key);
protected:
	virtual void Check();

	int now_key;
	std::function<void()> end_func;
	Player* player;
	UIManager* ui_manager;
	NpcDef npc_def;
};

class FuncNpc : public Npc
{
public:
	FuncNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, ItemManager* item_manager,
		UIManager* ui_manager, Player* player, int id);

	virtual void Init() override = 0;
	virtual void Stop() override;
	virtual void Use() override = 0;
	void Update(int choose_key) override;
protected:
	virtual void Check() override;

	ItemManager* item_manager;
	int choose;
	bool is_func;
};

class TradeNpc : public FuncNpc
{
public:
	TradeNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const TraderDef& trade_def, const NpcDef& npc_def,
		ItemManager* item_manager, UIManager* ui_manager, Player* player, int id);
	void Init() override;
	void Use() override;
private:

	TraderDef trade_def;
};

class TravelerNpc : public FuncNpc
{
public:
	TravelerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
		UIManager* ui_manager, ItemManager* item_manager, Player* player, int id);
	void Init() override;
	void Use() override;
};

class ChangerNpc : public FuncNpc
{
public:
	ChangerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* ui_manager,
		ItemManager* item_manager, Player* player, int id);
	void Init() override;
	void Use() override;
private:
	int coef;
};

class PortalNpc : public FuncNpc
{
public:
	PortalNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* ui_manager,
		ItemManager* item_manager, Player* player, int id);
	void Init() override;
	void Use() override;
};

class MechanicNpc : public FuncNpc
{
public:
	MechanicNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* ui_manager,
		ItemManager* item_manager, Player* player, int id);
	void Init() override;
	void Use() override;
	virtual void Stop() override;
private:
	void Check() override;

	int type_upgade;
};

class QuestNpc : public FuncNpc
{
public:
	QuestNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* ui_manager,
		ItemManager* item_manager, Player* player, int id);
	void Init() override;
	void Use() override;
private:
	void Check() override;
};

#endif // !SPRITE
