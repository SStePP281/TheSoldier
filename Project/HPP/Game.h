#ifndef GAME
#define GAME

#include "Sprite.h"
#include "DataBase.h"
#include "DialogSystem.h"
#include "Player.h"
#include "Renderer.h"
#include "SpriteManager.h"
#include "ItemManager.h"
#include "UIManeger.h"
#include "Inventory.h"
#include "MapManager.h"
#include "SoundManager.h"
#include "RenderState.h"
#include "Menu.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <memory>
#include <vector>
#include <Windows.h>

class Game
{
public:
public:
	Game(sf::RenderWindow* window, MapManager* map_manager);
	~Game();
	void GetInput(const sf::Event& event, float delta_time);
	void MakeCycle(float delta_time);
	void Save();
	void Editor(); 
private:
	void Init();
	void SubscribeEvent();
	void ResetGame();
	void GetInput(float delta_time);
	void InitPlayer();

	bool is_key_pressed;
	Menu* menu;
	RenderState* current_state;
	RenderState play_state;
	Inventory* inventory;
	Dialog* dialog_system;
	sf::Vector2i screen_midle_position;
	SpriteManager* sprite_manager;
	UIManager* ui_manager;
	ItemManager* item_manager;
	Player* player;
	Renderer* renderer;
	MapManager* map_manager;
	sf::RenderWindow* window;
};

#endif // !GAME
