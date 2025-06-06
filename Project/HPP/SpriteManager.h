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
	SpriteManager(Map* now_map, UIManager* ui_manager, ItemManager* item_manager);
	~SpriteManager();
	void Update(float delta_time);
	void ResetMap(Map* new_map, const sf::Vector2f& player_position);
	void ResetOldPlayer();
	Player* GetPlayer();
	Npc* GetNpc(int id);
	std::vector<std::shared_ptr<Sprite>>* GetDeteachSprite();
private:
	void Init();
	void CreateDecor(const MapSprite& map_sprite, const SpriteDef& sprite_def);
	void CreateSpriteFromMapSprite(const MapSprite& map_sprite);
	void CreateBoss(const MapSprite& map_sprite, const SpriteDef& sprite_def);
	void SpawnPortal(const sf::Vector2f& position);
	void CreateConverter(const MapSprite& map_sprite, const SpriteDef& sprite_def);
	void CreateEnemy(const MapSprite& map_sprite, const SpriteDef& sprtie_def);
	void CreateNpc(const MapSprite& map_sprite, const SpriteDef&);
	void CreatePlayer(const MapSprite& map_sprite, const SpriteDef& sprtie_def, const PlayerDef& player_def);
	void CreateDefaultPlayer(const PlayerDef& plaayer_def);
	void AIControler(float delta_time);
	void KillEnemy(Enemy* enem);
	bool IsEnemyHit(Enemy* enemy, float distance);
	void SpawnEnemy(const std::pair<int, sf::Vector2i>& pair);

	int id;
	UIManager* ui_manager;
	ItemManager* item_manager;
	std::vector<std::shared_ptr<Sprite>>* all_sprites;
	std::vector<Enemy*> enemys;
	std::unique_ptr<Player> player;
	Map* now_map;
};

#endif // !SPMANAGER
