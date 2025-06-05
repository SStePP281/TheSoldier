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
	void Update(float deltaTime);
	void ResetMap(Map* newMap, const sf::Vector2f& playerPos);
	void ResetOldPlayer();
	Player* GetPlayer();
	Npc* GetNpc(int id);
	std::vector<std::shared_ptr<Sprite>>* GetDeteachSprite();
private:
	void Init();
	void CreateDecor(const MapSprite& mapSprite, const SpriteDef& spDef);
	void CreateSpriteFromMapSprite(const MapSprite& mapSprite);
	void CreateBoss(const MapSprite& spMap, const SpriteDef& spDef);
	void SpawnPortal(const sf::Vector2f& pos);
	void CreateConverter(const MapSprite& mapSprite, const SpriteDef& def);
	void CreateEnemy(const MapSprite& mapSprite, const SpriteDef& def);
	void CreateNpc(const MapSprite& mapSprite, const SpriteDef&);
	void CreatePlayer(const MapSprite& mapSprite, const SpriteDef& def, const PlayerDef& plDef);
	void CreateDefaultPlayer(const PlayerDef& plDef);
	void AIControler(float deltaTime);
	void KillEnemy(Enemy* enem);
	bool IsEnemyHit(Enemy* enemy, float distance);
	void SpawnEnemy(const std::pair<int, sf::Vector2i>& pair);

	int id;
	UIManager* uiManager;
	ItemManager* itemManager;
	std::vector<std::shared_ptr<Sprite>>* allSprites;
	std::vector<Enemy*> enemys;
	std::unique_ptr<Player> player;
	Map* nowMap;
};

#endif // !SPMANAGER
