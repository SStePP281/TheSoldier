#pragma once
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
	Editor() = default;
	void init(sf::RenderWindow* window, sf::RenderWindow* editorWindow, MapManager* mapMn);
	void takeWindowInput(sf::Event& event);
	void takeEditInput(sf::Event& event);
	void drawEditor();

	int drawerLayer();
private:
	sf::RenderWindow* window;
	sf::RenderWindow* editorWindow;
	bool isFirstMouse;
	int nowValue;
	int nowLayer;
	SpriteDef nowSpriteDef;
	sf::Vector2i lastMousePos;
	sf::Vector2i windowMousePos, editorMousePos;
	sf::View windowView, editorView;
	MapManager* mapManager;
	std::vector<std::shared_ptr<Button>> buttons;

	void createTextureButton();
	void createSpriteButton();
	void windowStateRightClick();
	void scrollAndCntr(float delta);
	void windowStateNoRightClick();
	void windowStateLeftClick();
	void editorWindowStateLeftClick();
	sf::Vector2i getMapPos(sf::Vector2f& worldPos);
};

#endif // !EDITOR