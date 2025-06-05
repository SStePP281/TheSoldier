#ifndef MENU
#define MENU

#include "RenderState.h"
#include "EventSystem.h"
#include "UIManeger.h"

class Menu
{
public:
	Menu(sf::RenderWindow* window, UIManager* ui_manager, Player* player);
	void InitStartMenu();
	void InitGameMenu();
	void InitSetting();
	void InitResetMenu();
	void Stop();
	void Draw();
	void UpdateResetMenu();
	void UpdateStartMenu();
	void UpdateGameMenu();
	void UpdateSetting();
private:
	bool is_key_pressed;
	sf::RenderWindow* window;
	Player* player;
	UIManager* ui_manager;
	RenderState reset_state;
	RenderState start_menu_state;
	RenderState game_menu_state;
	RenderState setting_state;
};

#endif // !MENU
