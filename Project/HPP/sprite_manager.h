#ifndef SPMANAGER
#define SPMANAGER

#include "resources.h"
#include "raycast.h"
#include "sprite.h"
#include "player.h"
#include "map.h"
#include "data_base.h"
#include "map_manager.h"

class SpriteManager {
public:
	SpriteManager(Map* const now_map, UIManager* const ui_manager, ItemManager* const item_manager);
	~SpriteManager();
	void Update(float delta_time);
	void ResetMap(Map* const new_map, const sf::Vector2f& player_position);
	void ResetOldPlayer();
	Player* const GetPlayer() const;
	Npc* const GetNpc(int id) const;
	std::vector<std::shared_ptr<Sprite>>* const GetDeteachSprite() const;
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
	void AIControler(float delta_time) const;
	void KillEnemy(Enemy* const enem);
	bool IsEnemyHit(Enemy* const enemy, float distance) const;
	void SpawnEnemy(const std::pair<int, sf::Vector2i>& pair);

	int id_;
	UIManager* const ui_manager_;
	ItemManager* const item_manager_;
	std::vector<std::shared_ptr<Sprite>>* all_sprites_;
	std::vector<Enemy*> enemys_;
	std::unique_ptr<Player> player_;
	Map* now_map_;
};

#endif // !SPMANAGER
