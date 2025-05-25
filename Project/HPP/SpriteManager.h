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
	void resetMap(Map* newMap, sf::Vector2f& playerPos);
	void resetOldPlayer();
	Player* getPlayer();
	Npc* getNpc(int id);
	std::vector<std::shared_ptr<Sprite>>* getDeteachSprite();
private:
	void init();
	void createDecor(MapSprite& mapSprite, SpriteDef& spDef);
	void createSpriteFromMapSprite(MapSprite& mapSprite);
	void createBoss(MapSprite& spMap, SpriteDef& spDef);
	void spawnPortal(const sf::Vector2f& pos);
	void createConverter(MapSprite& mapSprite, SpriteDef& def);
	void createEnemy(MapSprite& mapSprite, SpriteDef& def);
	void createNpc(MapSprite& mapSprite, SpriteDef&);
	void createPlayer(MapSprite& mapSprite, SpriteDef& def, PlayerDef& plDef);
	void createDefaultPlayer(PlayerDef& plDef);
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
