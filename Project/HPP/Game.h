#pragma once
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
	Game(sf::RenderWindow* window, MapManager* mapManager);
	~Game();
	void getInput(sf::Event& event, float deltaTime);
	void makeCycle(float deltaTime);
	void save();
	void editor(); 
private:
	void init();
	void subscribeEvent();
	void resetGame();
	void getInput(float deltaTime);
	void initPlayer();

	bool isKeyPressed;
	Menu* menu;
	RenderState* currentState;
	RenderState playState;
	Inventory* invent;
	Dialog* dialogSys;
	sf::Vector2i screenMidlePos;
	SpriteManager* spManager;
	UIManager* uiManager;
	ItemManager* itemManager;
	Player* player;
	Renderer* renderer;
	MapManager* mapManager;
	sf::RenderWindow* window;
};

#endif // !GAME
