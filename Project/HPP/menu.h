#ifndef MENU
#define MENU

#include "render_state.h"
#include "event_system.h"
#include "ui_maneger.h"

class Menu {
public:
	Menu(sf::RenderWindow* window, UIManager* const ui_manager, Player* const player);
	void InitStartMenu();
	void InitGameMenu();
	void InitSetting();
	void InitResetMenu();
	void Stop();
	void Draw() const;
	void UpdateResetMenu();
	void UpdateStartMenu();
	void UpdateGameMenu();
	void UpdateSetting();
private:
	bool is_key_pressed_;
	sf::RenderWindow* window_;
	Player* const player_;
	UIManager* const ui_manager_;
	RenderState reset_state_;
	RenderState start_menu_state_;
	RenderState game_menu_state_;
	RenderState setting_state_;
};

#endif // !MENU
