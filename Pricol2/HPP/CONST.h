#pragma once
#ifndef CONSTS
#define CONSTS

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <iostream>

//#define REDACT_MODE

#define SQUARE(a) ((a) * (a))
#define GETDIST(a,b) (SQUARE(a.x - b.x) + SQUARE(a.y - b.y))
#define COMPARER(a, b, c) GETDIST(a, c) > GETDIST(b, c) ? true : false

constexpr float ROTATION_SPEED = 80.0f, VERTICAL_MOUSE_SPEED = 25.0f;
const int TEXTURE_SIZE = 128, ICON_SIZE = 64, TEXTURE_COUNT = 15;
constexpr int SCREEN_W = 1280, SCREEN_H = 720;
constexpr float PI = 3.14159265359f, TRIGER_DIST = 12.0f;
constexpr int ENEMY_MAX_INDEX = 12, PORTAL_INDEX = 17, SPAWN_RADIUS = 8;
constexpr int MAX_RAD = 30, MIN_RAD = 1;
constexpr int COUNT_ROW_TEXT = 7;
constexpr int MIN_LEAF_SIZE = 10, MAX_LEAF_SIZE = 20;
constexpr int ALL_LAYER = 4;
constexpr int LAYER_COUNT = 3;
constexpr int FLOOR_LAYER = 0;
constexpr int WALL_LAYER = 1;
constexpr int CELL_LAYER = 2;
constexpr int SPRITE_LAYER = 3;
constexpr int SPACE_SIZE_W = 40, SPACE_SIZE_H = 40;
constexpr int ENEMY_LEVEL_COUNT = 40;
constexpr int NEXT_LEVEL_N = -1, BASE_N = 0, ARENA_1_N = 1, ARENA_2_N = 2, ARENA_3_N = 3, BOSS_N = 4;
constexpr int MAX_DETH = 64;
constexpr float PLAYER_FOV = 60.0f, CAMERA_Z = SCREEN_H / 2.0f, ASPECT = (float)SCREEN_W / SCREEN_H * 0.5f, BRIGHTNESTDIST = MAX_DETH / 7.0f;
constexpr float DIALOG_W = SCREEN_W - 200, DIALOG_H = SCREEN_H - 100, INTERVAL = 40;
constexpr int TEXTSIZE = 50;

enum class SpriteType { Enemy, PlayerT, NPC, Convertor, Boss, Decoration };

enum EnemyState { Stay, Run, Attack, Spawn, Dead };

enum NpcType { Dilog, TraderNpcType, Traveler, ChangerNpcType, QuestNpcType, MechanicNpcType, PortalNpcType };

enum QuestType { KillMonster, CollectionMoney, CollectionDetails };

enum MusicType { MenuSound, BaseSound, LevelSound, StartIntro, EndIntro };

enum ImproveType { Damage, Spread, Magazin };

enum ItemType { MaxEnergy, MaxHeal, Heal, Armor, Patrons };

struct QuestData
{
	QuestType type;
	int progress;
	int target;
	int rewardCoins;
};

struct PlayerDef
{
	float maxHp;
	float nowHp;
	float maxEnergy;
	float nowEnergy;
	float defence;
	float maxStrenght;
	float nowStrenght;
	int countpantrons;
	int money;
	int details;
	std::vector<int> gunsData;
};

struct GunData
{
	int id;
	int nowCount;
	int nowMaxCount;
	int nowMaxRad;
	int nowDamage;
	int upgradeCount;
	std::vector<int> improveId;
};

struct GunDef
{
	int id;
	int damage;
	int maxCount;
	int nowCount;
	float shutTime;
	float maxDist;
	float resetTime;
	int cost;
	std::wstring name;
	std::wstring disc;
};

struct ItemsDef
{
	int id;
	ItemType type;
	std::wstring name;
	int effect;
	int maxUSing;
	int cost;
	std::wstring disc;
};

struct ImproveDef
{
	int id;
	ImproveType type;
	std::wstring name;
	float effect;
	int cost;
	std::wstring disc;
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
	int idKey;
};

struct TraderDef
{
	int startKey;
	std::vector<int> title;
};

struct GameStateData
{
	bool isFirstGame;
	int effectVolume;
	int soundVolume;
	int levelNumber;
	bool isLevelBase;
	int changerCoef;
	float mouseSpeed;
	bool killFirst;
	bool killSecond;
	bool killTherd;
};

static std::vector<std::string> mapFileNames {
	"base.map",
	"converter1.map",
	"converter2.map",
	"converter3.map",
	"boss.map" 
};

static std::vector<ItemsDef> itemsDefs {
	{9,  ItemType::Heal, L"Малая аптечка", 20, 1, 10, L"Востанавливает 20 HP"},
	{10, ItemType::Heal, L"Средняя аптечка", 50, 1, 20, L"Востанавливает 50 HP"},
	{11, ItemType::Heal, L"Большая аптечка", 100, 1, 30, L"Востанавливает 100 HP"},
	{12, ItemType::MaxHeal, L"Эксперементальное снадобье 1", 20, 1, 300, L"80% что +20 к maxHP, 20% - -10 maxHP"},
	{13, ItemType::MaxEnergy, L"Эксперементальное снадобье 2", 2, 1, 300, L"80% что +2 к maxEnergy, 20% - -1 maxEnergy"},
	{14, ItemType::Armor, L"Железная пластина", 10, 50, 50, L"-10% получаемого урона, 50 использований"},
	{15, ItemType::Armor, L"Стальная пластина", 20, 70, 80, L"-20% получаемого урона, 70 использований"},
	{16, ItemType::Armor, L"Кевларовая пластина", 30, 90, 100, L"-30% получаемого урона, 90 использований"},
	{17, ItemType::Armor, L"Карбоновая пластина", 50, 110, 200, L"-50% получаемого урона, 110 использований"},
	{18, ItemType::Patrons, L"Малый ящик патронов", 50, 1, 20, L"Содержит 50 патронов"},
	{19, ItemType::Patrons, L"Средний ящик патронов", 100, 1, 35, L"Содержит 100 патронов"},
	{20, ItemType::Patrons, L"Большой ящик патронов", 150, 1, 50, L"Содержит 150 патронов"}
};

static std::vector<GunDef> gunsDef {
	{21, 10, 100, 100, 0.5f, 2.0f, 0, 0,       L"Нога",        L"Урон - 10  | Обойма - 100 пт | Скорострельность - 0.5 сек | Дистанция - 1  м"},
	{22, 10, 100, 100, 0.3f, 2.0f, 0, 0,       L"Кулак",       L"Урон - 10  | Обойма - 100 пт | Скорострельность - 0.3 сек | Дистанция - 1  м"},
	{23, 10, 20, 20, 0.4f, 20.0f, 1.5f, 0,     L"Пистолет",    L"Урон - 10  | Обойма - 20  пт | Скорострельность - 0.4 сек | Дистанция - 20 м"},
	{24, 50, 5, 5, 1.0f, 5.0f, 2.0f, 340,      L"Обрез",       L"Урон - 50  | Обойма - 5   пт | Скорострельность - 1.0 сек | Дистанция - 5  м"},
	{25, 90, 2, 2, 0.6f, 5.0f, 3.0f, 500,      L"Двухстволка", L"Урон - 90  | Обойма - 2   пт | Скорострельность - 0.6 сек | Дистанция - 5  м"},
	{26, 20, 30, 30, 0.4f, 15.0f, 3.0f, 600,   L"Автомат",     L"Урон - 20  | Обойма - 30  пт | Скорострельность - 0.4 сек | Дистанция - 15 м"},
	{27, 200, 1, 1, 1.0f, 10.0f, 3.0f, 700,    L"Базука",      L"Урон - 200 | Обойма - 1   пт | Скорострельность - 1.0 сек | Дистанция - 10 м"},
	{28, 10, 100, 100, 0.1f, 10.0f, 4.0f, 800, L"Миниган",     L"Урон - 10  | Обойма - 100 пт | Скорострельность - 0.1 сек | Дистанция - 10 м"},
};

static std::vector<ImproveDef> improveDefs {
	{0, ImproveType::Magazin, L"Малый магазин", 1.2f, 200,   L"+20% к объему магазина"},
	{1, ImproveType::Magazin, L"Средний магазин", 1.3f, 300, L"+30% к объему магазина"},
	{2, ImproveType::Magazin, L"Большой магазин", 1.5f, 400, L"+50% к объему магазина"},
	{3, ImproveType::Spread,  L"Лазерный прицел", 1.4f, 150, L"-40% к разбросу"},
	{4, ImproveType::Spread,  L"Голографический прицел", 1.6f, 250, L"-60% к разбросу"},
	{5, ImproveType::Spread,  L"Оптический прицел", 1.8f, 350, L"-80% к разбросу"},
	{6, ImproveType::Damage,  L"Пламягаситель", 1.3f, 250, L"+30% к урону"},
	{7, ImproveType::Damage,  L"Упор", 1.4f, 350, L"+40% к урону"},
	{8, ImproveType::Damage,  L"Приклад", 1.5f, 450, L"+50% к урону"}
};

static std::vector<ItemsDef> travelerDefs {
	{29, ItemType::Heal, L"", NEXT_LEVEL_N, 1, 50,  L"Перемещение на следующий уровень"},
	{30, ItemType::Heal, L"", ARENA_1_N,    1, 100, L"Перемещение на преобразоваетль 1"},
	{31, ItemType::Heal, L"", ARENA_2_N,    1, 150, L"Перемещение на преобразоваетль 2"},
	{32, ItemType::Heal, L"", ARENA_3_N,    1, 200, L"Перемещение на преобразоваетль 3"},
	{33, ItemType::Heal, L"", BOSS_N,       1, 250, L"Поездка к боссу"}
};

static std::vector<NpcDef> npcDefs{
	{PortalNpcType, 1},
	{TraderNpcType, 4},
	{TraderNpcType, 5},
	{ChangerNpcType, 3},
	{Traveler, 2},
	{QuestNpcType, 6},
	{MechanicNpcType, 7},
	{NpcType::Dilog, 8},
	{NpcType::Dilog, 9},
	{NpcType::Dilog, 10},
	{NpcType::Dilog, 11},
	{NpcType::Dilog, 12},
	{NpcType::Dilog, 13},
};

static std::vector<TraderDef> traderDefs {
	{4, {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}},
	{5, {24, 25, 26, 27, 28, 0, 1, 2, 3, 4, 5, 6, 7, 8}}
};

static std::vector<EnemyDef> enemyDefs {
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
	{true,  3.0f,  30.0f, 10, 1.5f, 5.0f, 8000.0f},
	{false, 6.0f,  0.0f,  20, 3.0f, 0.0f, 100.0f},
	{false, 6.0f,  0.0f,  40, 3.0f, 0.0f, 100.0f},
	{false, 6.0f,  0.0f,  60, 3.0f, 0.0f, 100.0f}
};

static std::vector<ConverterDef> converterDefs {
	{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, 40},
	{{1, 2, 3, 4, 5}, 10},
	{{5, 6, 7, 8, 9}, 20},
	{{8, 9, 10, 11, 12}, 30}
};

static std::vector<SpriteDef> spriteDefs {
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
	{ L"Виктор",           SpriteType::NPC,       1.0f,  22, true},
	{ L"Сергей Петривоч",  SpriteType::NPC,       1.0f,  23, true},
	{ L"Марина Павловна",  SpriteType::NPC,       1.0f,  24, true},
	{ L"Дмитрий Сергеевич",SpriteType::NPC,       1.0f,  25, true},
	{ L"Нина Анатольевна", SpriteType::NPC,       1.0f,  26, true},
	{ L"Валера",           SpriteType::NPC,       1.0f,  27, true},
	{ L"Миша",             SpriteType::NPC,       1.0f,  28, true},
	{ L"Бочка",            SpriteType::Decoration,1.0f,  29, false},
	{ L"Мусор",            SpriteType::Decoration,1.0f,  30, false},
	{ L"Шины",             SpriteType::Decoration,1.0f,  31, false},
	{ L"Коробки",          SpriteType::Decoration,1.0f,  32, false},
};

#endif // !CONSTS
