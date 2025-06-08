#ifndef DIALOG
#define DIALOG

#include "ui_maneger.h"
#include "item_manager.h"
#include "render_state.h"
#include "event_system.h"
#include "player.h"
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>

class Dialog {
public:
	Dialog(sf::RenderWindow* window, UIManager* const ui_manager,
		ItemManager* weapon_manager);
	void SetPlayer(const Player* player);
	void Start(Npc* npc);
private:
	void Update() const;
	void Draw() const;
	void Stop();

	RenderState dialog_state_;
	Npc* npc_;
	Player* const player_;
	UIManager* ui_manager_;
	ItemManager* weapon_manager_;
	sf::RenderWindow* window_;
};

#endif // !DIALOG
