#pragma once
#ifndef SPRITE
#define SPRITE

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <functional>
#include <algorithm>
#include "Animation.h"
#include "Quest.h"
#include <memory>
#include <set>
#include <iostream>

constexpr float PI = 3.14159265359f, TRIGER_DIST = 12.0f;
constexpr int ENEMY_MAX_INDEX = 12, PORTAL_INDEX = 17, SPAWN_RADIUS = 8;

class Map;
class Gun;
class Player;
class UIManager;
class ItemManager;

enum class SpriteType
{
	Enemy, PlayerT, NPC, Convertor, Boss
};

enum EnemyState
{
	Stay, Run, Attack, Spawn, Dead
};

enum NpcType
{
	Dilog, TraderNpcType, Traveler, ChangerNpcType, QuestNpcType, MechanicNpcType, PortalNpcType
};

struct MapSprite
{
	int spriteDefId;
	sf::Vector2f position;
	float angle;
	float nowHealPoint;
};

struct SpriteDef
{
	std::wstring name;
	SpriteType type;
	float size;
	int texture;
	bool isDirectional;
};

struct EnemyDef
{
	bool isCanRun;
	float attackDist;
	float damage;
	int midleDrop;
	float timeBettwenAtack;
	float speed;
	float maxHealpoint;
};

struct ConverterDef
{
	std::vector<int> callingIndex;
	int maxSpawnCount;
};

struct NpcDef
{
	NpcType type;
	int startKey;
};

struct TraderDef
{
	int startKey;
	std::vector<int> title;
};

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

static std::vector<NpcDef> npcDefs = {
	{PortalNpcType, 1},
	{TraderNpcType, 4},
	{TraderNpcType, 5},
	{ChangerNpcType, 3},
	{Traveler, 2},
	{QuestNpcType, 6},
	{MechanicNpcType, 7}
};

static std::vector<TraderDef> traderDefs = {
	{4, {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}},
	{5, {24, 25, 26, 27, 28, 29, 0, 1, 2, 3, 4, 5, 6, 7, 8}}
};

static std::vector<EnemyDef> enemyDefs = {
	{true,  0.0f,   0.0f,  0,  0.0f, 5.0f, 0.0f },
	{true,  3.0f,  5.0f,  3, 1.5f, 3.0f, 70.0f  },
	{true,  3.0f,  8.0f,  5, 1.0f, 4.0f, 90.0f  },
	{true,  3.0f,  10.0f, 7, 1.5f, 5.0f, 100.0f },
	{true,  7.0f,  8.0f,  9, 1.0f, 4.0f, 120.0f },
	{true,  7.0f,  10.0f, 10, 1.0f, 5.0f, 200.0f },
	{true,  3.0f,  12.0f, 13, 1.5f, 6.0f, 150.0f },
	{true,  7.0f,  20.0f, 14, 1.5f, 3.0f, 300.0f },
	{true,  3.0f,  15.0f, 15, 1.5f, 6.0f, 200.0f },
	{true,  3.0f,  20.0f, 18, 1.5f, 6.0f, 200.0f },
	{true,  7.0f,  22.0f, 19, 1.0f, 5.0f, 180.0f },
	{false, 7.0f,  35.0f, 20, 1.5f, 4.0f, 300.0f },
	{true,  3.0f,  26.0f, 25, 1.5f, 4.0f, 320.0f },
	{true,  3.0f,  30.0f, 10, 1.5f, 5.0f, 2000.0f},
	{false, 6.0f,  0.0f,  20, 3.0f, 0.0f, 100.0f},
	{false, 6.0f,  0.0f,  40, 3.0f, 0.0f, 100.0f},
	{false, 6.0f,  0.0f,  60, 3.0f, 0.0f, 100.0f}
};

static std::vector<ConverterDef> converterDefs = {
	{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, 5},
	{{1, 2, 3, 4, 5}, 10},
	{{5, 6, 7, 8, 9}, 20},
	{{8, 9, 10, 11, 12}, 30}
};

static std::vector<SpriteDef> spriteDefs = {
	{ L"player",           SpriteType::PlayerT,   0.3f, -1,  false},
	{ L"Бабл",             SpriteType::Enemy,     1.0f,  0,  true},
	{ L"Синчик",           SpriteType::Enemy,     1.0f,  1,  true},
	{ L"Розовый пинки",    SpriteType::Enemy,     1.0f,  2,  true},
	{ L"Кибер демон",      SpriteType::Enemy,     1.0f,  3,  true},
	{ L"Спайдер",          SpriteType::Enemy,     1.0f,  4,  true},
	{ L"Красный череп",    SpriteType::Enemy,     1.0f,  5,  true},
	{ L"Бомбастик",        SpriteType::Enemy,     1.0f,  6,  true},
	{ L"Зеленый пинки",    SpriteType::Enemy,     1.0f,  7,  true},
	{ L"Розовый череп",    SpriteType::Enemy,     1.0f,  8,  true},
	{ L"Ревенант",         SpriteType::Enemy,     1.0f,  9,  true},
	{ L"Мега бабл",        SpriteType::Enemy,     1.0f,  10, true},
	{ L"Мать",             SpriteType::Enemy,     1.0f,  11, true},
	{ L"Boss",             SpriteType::Boss,      1.0f,  12, true},
	{ L"Преобразователь1", SpriteType::Convertor, 1.0f,  13, true},
	{ L"Преобразователь2", SpriteType::Convertor, 1.0f,  14, true},
	{ L"Преобразователь3", SpriteType::Convertor, 1.0f,  15, true},
	{ L"Портал",           SpriteType::NPC,       1.0f,  16, true},
	{ L"Петрович",         SpriteType::NPC,       1.0f,  17, true},
	{ L"Молотов",          SpriteType::NPC,       1.0f,  18, true},
	{ L"Роман",            SpriteType::NPC,       1.0f,  19, true},
	{ L"Ванька",           SpriteType::NPC,       1.0f,  20, true},
	{ L"Тихон",            SpriteType::NPC,       1.0f,  21, true},
	{ L"Виктор",           SpriteType::NPC,       1.0f,  22, true} 
};

#endif // !SPRITE
