#ifndef SPRITE
#define SPRITE

#include "const.h"
#include "animation.h"
#include "quest.h"
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

class Sprite {
public:
	Sprite(const SpriteDef& sprite_def, const MapSprite& map_sprite, int id);
	Sprite() = default;
	virtual ~Sprite() = default;
	virtual const std::pair<int, bool> GetTextIndex() const;
	void SetPosition(const sf::Vector2f& new_position);
	const sf::Vector2f& GetPosition() const;
	const float GetTextureSize() const;
	void SetAngle(float new_angle);
	const float GetAngle() const;
	const float GetSize() const;
	const int GetId() const;
	const SpriteType& GetSpriteType() const;
	const MapSprite& GetMapSprite() const;
	const sf::Texture* const GetTexture() const;
	const int GetTextureId() const;
	void SetTextureSize(int new_size);
	const bool IsDirectional() const;
	void SetBlockmapCoords(std::set<std::tuple<int, int>> coords);
	const std::set<std::tuple<int, int>>& GetBlockmapCoords() const;
protected:
	int texture_size_;
	SpriteDef sprite_def_;
	MapSprite map_sprite_;
	sf::Texture* texture_;
	std::set<std::tuple<int, int>> blockmap_coords_;
private:
	int id_;
};

class Enemy : public Sprite {
public:
	Enemy(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, int id);
	virtual ~Enemy() = default;
	virtual const std::pair<int, bool> GetTextIndex() const override;
	virtual void Death();
	virtual void Attack(Player* const player);
	void Update(float delta_time);
	void Move(Map* const map, const sf::Vector2f& move);
	virtual void TakeDamage(float damage);
	virtual void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time);
	virtual void ChangeState(EnemyState new_state);
	void SetHealpoint(float now_healpoint);
	const float GetHealpoint() const;
	void SetMaxHealPoint(float new_healpoint);
	const float GetMaxHealpoint() const;
	const float GetMoveSpeed() const;
	const float GetAttackDistance() const;
	const int GetMidleDrop() const;
	void SetIsCanAttcak(bool value);
	const bool GetIsCanAttack() const;
	void SetIsAttcak(bool value);
	const bool GetIsAttack() const;
protected:
	void UpdateTimeSinceLastAttack(float delta_time);
	void UpdateTimeSinceDamaged(float delta_time);
	const bool CheckCollision(Map* const map, const sf::Vector2f& new_pos, bool x_axis) const;
	virtual const EnemyState DetermineNewState(float dist) const;
	virtual const bool CanChangeState() const;

	bool is_damaged_;
	bool is_can_attack_;
	bool is_attack_;
	EnemyState state_;
	EnemyDef enemy_def_;
	Animator<int> animr_;
	float time_atecked_, now_time_attack_;
};

class Converter : public Enemy {
public:
	Converter(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& converter_def, int id);
	void TakeDamage(float damege) override;
	void Death() override;
	void Attack(Player* const plaer) override;
	void ChangeState(EnemyState new_state) override;
	const bool CanChangeState() const override;
	void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time) override;
	const EnemyState DetermineNewState(float dist) const override;
protected:
	int now_spawn_count_;
	ConverterDef converter_def_;
};

class Boss : public Enemy {
public:
	Boss(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& converter_def, int id);
	void Death() override;
	void Attack(Player* const plaer) override;
	void ChangeState(EnemyState new_state) override;
	void EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time) override;
	const EnemyState DetermineNewState(float dist) const override;
private:
	int now_spawn_count_;
	ConverterDef converter_def_;
};

class Npc : public Sprite {
public:
	Npc(const SpriteDef& sprite_def, const MapSprite& map_sprite, UIManager* const ui_manager, Player* const player, const NpcDef& npc_def, int id);
	Npc() = default;
	virtual ~Npc() = default;
	void SetEndFunc(std::function<void()>&& endFunc);
	virtual void Init();
	virtual void Stop();
	virtual void Use();
	virtual void Update(int choose_key);
protected:
	virtual void Check();

	int now_key_;
	std::function<void()> end_func_;
	Player* const player_;
	UIManager* const ui_manager_;
	NpcDef npc_def_;
};

class FuncNpc : public Npc {
public:
	FuncNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, ItemManager* const item_manager,
		UIManager* const ui_manager, Player* const player, int id);
	virtual void Init() override = 0;
	virtual void Stop() override;
	virtual void Use() override = 0;
	void Update(int choose_key) override;
protected:
	virtual void Check() override;

	ItemManager* const item_manager_;
	int choose_;
	bool is_func_;
};

class TradeNpc : public FuncNpc {
public:
	TradeNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const TraderDef& trade_def, const NpcDef& npc_def,
		ItemManager* const item_manager, UIManager* const ui_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
private:
	TraderDef trade_def_;
};

class TravelerNpc : public FuncNpc {
public:
	TravelerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
		UIManager* const ui_manager, ItemManager* const item_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
};

class ChangerNpc : public FuncNpc {
public:
	ChangerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* const ui_manager,
		ItemManager* const item_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
private:
	int coef_;
};

class PortalNpc : public FuncNpc {
public:
	PortalNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* const ui_manager,
		ItemManager* const item_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
};

class MechanicNpc : public FuncNpc {
public:
	MechanicNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* const ui_manager,
		ItemManager* const item_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
	virtual void Stop() override;
private:
	void Check() override;

	int type_upgade_;
};

class QuestNpc : public FuncNpc {
public:
	QuestNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, UIManager* const ui_manager,
		ItemManager* const item_manager, Player* const player, int id);
	void Init() override;
	void Use() override;
private:
	void Check() override;
};

#endif // !SPRITE
