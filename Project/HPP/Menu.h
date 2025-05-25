#pragma once
#ifndef MENU
#define MENU

#include "RenderState.h"
#include "EventSystem.h"
#include "UIManeger.h"

class Menu
{
public:
	Menu(sf::RenderWindow* _window, UIManager* _uiManager, Player* player);
	void initStartMenu();
	void initGameMenu();
	void initSetting();
	void initResetMenu();
	void stop();
	void draw();
	void updateResetMenu();
	void updateStartMenu();
	void updateGameMenu();
	void updateSetting();
private:
	bool isKeyPressed;
	sf::RenderWindow* window;
	Player* player;
	UIManager* uiManager;
	RenderState resetState;
	RenderState startMenuState;
	RenderState gameMenuState;
	RenderState settingState;
};
#endif // !MENU