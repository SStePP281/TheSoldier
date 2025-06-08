#ifndef EDITOR
#define EDITOR

#include "map_manager.h"
#include "resources.h"
#include "sfml_extantion.h"
#include "const.h"
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

class Editor {
public:
	Editor(sf::RenderWindow* window, sf::RenderWindow* editor_window, MapManager* const map_manager);
	void TakeWindowInput(sf::Event& event);
	void TakeEditInput(sf::Event& event);
	void DrawEditor() const;
	const int DrawerLayer() const;
private:
	void CreateTextureButton();
	void CreateSpriteButton();
	void WindowStateRightClick();
	void ScrollAndCntr(float delta) const;
	void WindowStateNoRightClick();
	void WindowStateLeftClick() const;
	void EditorWindowStateLeftClick() const;
	const sf::Vector2i& GetMapPos(const sf::Vector2f& world_position) const;

	sf::RenderWindow* window_;
	sf::RenderWindow* editor_window_;
	bool is_first_mouse_;
	int now_value_;
	int now_layer_;
	SpriteDef now_sprite_def_;
	sf::Vector2i last_mouse_pos_;
	sf::Vector2i window_mouse_position_, editor_mouse_position_;
	sf::View window_view_, editor_view_;
	MapManager* const map_manager_;
	std::vector<std::shared_ptr<Button>> buttons_;
};

#endif // !EDITOR
