#ifndef DIALOG
#define DIALOG

#include "UIManeger.h"
#include "ItemManager.h"
#include "RenderState.h"
#include "EventSystem.h"
#include "Player.h"
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>

class Dialog
{
public:
	Dialog(sf::RenderWindow* window, UIManager* ui_manager, 
		ItemManager* weapon_manager);
	void SetPlayer(Player* player);
	void Start(Npc* npc);
private:
	void Update();
	void Draw();
	void Stop();

	RenderState dialog_state;
	Npc* npc;
	Player* player;
	UIManager* ui_manager;
	ItemManager* weapon_manager;
	sf::RenderWindow* window;
};

#endif // !DIALOG
