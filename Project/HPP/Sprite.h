#pragma once
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
	Sprite(SpriteDef& spDef, MapSprite& spMap, int _id);
	Sprite() = default;
	virtual ~Sprite() = default;
	virtual std::pair<int, bool> getTextIndex();

	int id;
	int textSize;
	SpriteDef spDef;
	MapSprite spMap;
	sf::Texture* texture;
	std::set<std::tuple<int, int>> blockmap_coords;
};

class Enemy : public Sprite
{
public:
	Enemy(SpriteDef& spDef, MapSprite& spMap, EnemyDef& enemyDef, int id);
	virtual ~Enemy() = default;
	virtual std::pair<int, bool> getTextIndex() override;
	virtual void death();
	virtual void attack(Player* player);
	void update(float deltaTime);
	void move(Map* map, sf::Vector2f& move);
	virtual void takeDamage(float damage);
	virtual EnemyState determineNewState(float dist);
	virtual void enemyMechenic(float dist, sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime);
	virtual bool canChangeState();
	virtual void changeState(EnemyState newState);

	EnemyDef enemyDef;
	bool isCanAttack = false;
	bool isAtack = false;
	Animator<int> animr;
	float timeAtecked, nowTimeAtack;
protected:
	bool isDamaged;
	EnemyState state;

	void updateTimeSinceLastAttack(float deltaTime);
	void updateTimeSinceDamaged(float deltaTime);
	bool checkCollision(Map* map, sf::Vector2f& newPos, bool xAxis);
};

class Converter : public Enemy
{
public:
	Converter(SpriteDef& spDef, MapSprite& spMap, EnemyDef& enemyDef, ConverterDef& cDef, int id);
	void takeDamage(float damege) override;
	void death() override;
	void attack(Player* plaer) override;
	void changeState(EnemyState newState) override;
	bool canChangeState() override;
	void enemyMechenic(float dist, sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime) override;
	EnemyState determineNewState(float dist) override;

	ConverterDef cDef;
private:
	int nowSpawnCount;
};

class Boss : public Enemy
{
public:
	Boss(SpriteDef& spDef, MapSprite& spMap, EnemyDef& enemyDef, ConverterDef& cDef, int id);
	void death() override;
	void attack(Player* plaer) override;
	void changeState(EnemyState newState) override;
	void enemyMechenic(float dist, sf::Vector2f& toPlayerDir, Map* nowMap, float deltaTime) override;
	EnemyState determineNewState(float dist) override;

	ConverterDef cDef;
private:
	int nowSpawnCount;
};

class Npc : public Sprite
{
public:
	Npc(SpriteDef& spDef, MapSprite& spMap, UIManager* uiManager, Player* player, NpcDef& npcDef, int _id);
	Npc() = default;
	virtual ~Npc() = default;
	void setEndFunc(std::function<void()>&& _endFunc);
	virtual void init();
	virtual void stop();
	virtual void use();
	virtual void update(int chooseKey);
protected:
	virtual void check();

	int nowKey;
	std::function<void()> endFunc;
	Player* player;
	UIManager* uiManager;
	NpcDef npcDefData;
};

class FuncNpc : public Npc
{
public:
	FuncNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, ItemManager* itemManager, 
		UIManager* uiManager, Player* player, int id);

	virtual void init() override = 0;
	virtual void stop() override;
	virtual void use() override = 0;
	void update(int chooseKey) override;
protected:
	virtual void check() override;

	ItemManager* itemManager;
	int choose;
	bool isFunc;
};

class TradeNpc : public FuncNpc
{
public:
	TradeNpc(SpriteDef& spDef, MapSprite& spMap, TraderDef& tradeDef, NpcDef& npcDef,
		ItemManager* itemManager, UIManager* uiManager, Player* player, int _id);
	void init() override;
	void use() override;
private:

	TraderDef tradeDef;
};

class TravelerNpc : public FuncNpc
{
public:
	TravelerNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, 
		UIManager* uiManager, ItemManager* itemManager, Player* player, int _id);
	void init() override;
	void use() override;
};

class ChangerNpc : public FuncNpc
{
public:
	ChangerNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, UIManager* uiManager, 
		ItemManager* itemManager, Player* player, int _id);
	void init() override;
	void use() override;
private:
	int coef;
};

class PortalNpc : public FuncNpc
{
public:
	PortalNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, UIManager* uiManager,
		ItemManager* itemManager, Player* player, int _id);
	void init() override;
	void use() override;
};

class MechanicNpc : public FuncNpc
{
public:
	MechanicNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, UIManager* uiManager,
		ItemManager* itemManager, Player* player, int _id);
	void init() override;
	void use() override;
	virtual void stop() override;
private:
	void check() override;

	int typeUpgade;
};

class QuestNpc : public FuncNpc
{
public:
	QuestNpc(SpriteDef& spDef, MapSprite& spMap, NpcDef& npcDef, UIManager* uiManager,
		ItemManager* itemManager, Player* player, int _id);
	void init() override;
	void use() override;
private:
	void check() override;
};

#endif // !SPRITE
