#ifndef GAME
#define GAME

#include "sprite.h"
#include "data_base.h"
#include "dialog_system.h"
#include "player.h"
#include "renderer.h"
#include "sprite_manager.h"
#include "item_manager.h"
#include "ui_manager.h"
#include "inventory.h"
#include "map_manager.h"
#include "sound_manager.h"
#include "render_state.h"
#include "Menu.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <memory>
#include <vector>
#include <Windows.h>

class Game {
public:
	Game(sf::RenderWindow* window, MapManager* const map_manager);
	~Game();
	void GetInput(const sf::Event& event, float delta_time) const;
	void MakeCycle(float delta_time) const;
	void Save() const;
	void Editor() const;
private:
	void Init();
	void SubscribeEvent();
	void ResetGame() const;
	void GetInput(float delta_time) const;
	void InitPlayer() const;

	bool is_key_pressed_;
	Menu* menu_;
	RenderState* current_state_;
	RenderState play_state_;
	Inventory* inventory_;
	Dialog* dialog_system_;
	sf::Vector2i screen_midle_position_;
	SpriteManager* sprite_manager_;
	UIManager* ui_manager_;
	ItemManager* item_manager_;
	Player* player_;
	Renderer* renderer_;
	MapManager* const map_manager_;
	sf::RenderWindow* window_;
};

#endif // !GAME
