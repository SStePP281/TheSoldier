#pragma once
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
	Dialog(sf::RenderWindow* _window, UIManager* _uiManager, 
		ItemManager* _weaponManager);
	void setPlayer(Player* _player);
	void start(Npc* npc);
private:
	RenderState dialogState;
	Npc* npc;
	Player* player;
	UIManager* uiManager;
	ItemManager* weaponManager;
	sf::RenderWindow* window;

	void update();
	void draw();
	void stop();
};

#endif // !DIALOG