#pragma once
#ifndef SPMANAGER
#define SPMANAGER

#include "Resources.h"
#include "Raycast.h"
#include "Sprite.h"
#include "Player.h"
#include "Map.h"
#include "DataBase.h"
#include "MapManager.h"

class SpriteManager
{
public:
	SpriteManager(Map* _nowMap, UIManager* uiManager, ItemManager* _itemManager);
	~SpriteManager();
	void update(float deltaTime);
	void resetMap(Map* newMap, const sf::Vector2f& playerPos);
	void resetOldPlayer();
	Player* getPlayer();
	Npc* getNpc(int id);
	std::vector<std::shared_ptr<Sprite>>* getDeteachSprite();
private:
	void init();
	void createDecor(const MapSprite& mapSprite, const SpriteDef& spDef);
	void createSpriteFromMapSprite(const MapSprite& mapSprite);
	void createBoss(const MapSprite& spMap, const SpriteDef& spDef);
	void spawnPortal(const sf::Vector2f& pos);
	void createConverter(const MapSprite& mapSprite, const SpriteDef& def);
	void createEnemy(const MapSprite& mapSprite, const SpriteDef& def);
	void createNpc(const MapSprite& mapSprite, const SpriteDef&);
	void createPlayer(const MapSprite& mapSprite, const SpriteDef& def, const PlayerDef& plDef);
	void createDefaultPlayer(const PlayerDef& plDef);
	void aiControler(float deltaTime);
	void killEnemy(Enemy* enem);
	bool isEnemyHit(Enemy* enemy, float distance);
	void spawnEnemy(const std::pair<int, sf::Vector2i>& pair);

	int id;
	UIManager* uiManager;
	ItemManager* itemManager;
	std::vector<std::shared_ptr<Sprite>>* allSprites;
	std::vector<Enemy*> enemys;
	std::unique_ptr<Player> player;
	Map* nowMap;
};

#endif // !SPMANAGER
