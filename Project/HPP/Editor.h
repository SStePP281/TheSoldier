#ifndef EDITOR
#define EDITOR

#include "MapManager.h"
#include "Resources.h"
#include "sfmlExtantion.h"
#include "CONST.h"
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>

class Editor
{
public:
	Editor(sf::RenderWindow* window, sf::RenderWindow* editor_window, MapManager* map_manager);
	void TakeWindowInput(sf::Event& event);
	void TakeEditInput(sf::Event& event);
	void DrawEditor();

	int DrawerLayer();
private:
	void CreateTextureButton();
	void CreateSpriteButton();
	void WindowStateRightClick();
	void ScrollAndCntr(float delta);
	void WindowStateNoRightClick();
	void WindowStateLeftClick();
	void EditorWindowStateLeftClick();
	sf::Vector2i GetMapPos(const sf::Vector2f& world_position);

	sf::RenderWindow* window;
	sf::RenderWindow* editor_window;
	bool is_first_mouse;
	int now_value;
	int now_layer;
	SpriteDef now_sprite_def;
	sf::Vector2i last_mouse_pos;
	sf::Vector2i window_mouse_position, editor_mouse_position;
	sf::View window_view, editor_view;
	MapManager* map_manager;
	std::vector<std::shared_ptr<Button>> buttons;
};

#endif // !EDITOR
