#pragma once
#ifndef UIM
#define UIM

#include "Player.h"
#include "EventSystem.h"
#include "DataBase.h"
#include "sfmlExtantion.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>
#include <vector>
#include "Weapon.h"
#include <map>

constexpr float DIALOG_W = SCREEN_W - 200, DIALOG_H = SCREEN_H - 100, INTERVAL = 40;
constexpr int TEXTSIZE = 50;

class UIManager
{
public:
	UIManager(sf::RenderWindow* _window);
	void drawPlayerUI(Player* player);
	void initDialog(std::map<int, std::wstring, std::greater<int>>& variants,
		std::wstring& npcName);
	void initResetMenu();
	void initStartMenu();
	void initGameMenu();
	void initSetting();
	void initQuest(Quest* quest, Player* player);
	void initTrade(std::map<int, Itemble*>& variants, Player* player);
	void initMechanic(Player* player, Gun* choose);
	void initChanger(int coef, Player* player);
	void initInvent(std::map<Itemble*, int>& items, Itemble* chose, Player* player);
	void deleteNow();
	void drawNow();
	int checkButton();
private:
	std::wstring splitText(std::wstring text, int maxLen, int textSize);
	std::wstring toMax(std::wstring str, float maxW, float textSize);
	sf::RenderWindow* window;
	sf::Sprite background;
	Button* choseBut;
	std::vector<Button> buttons;
	int keyButton;
	std::function<void(Player* player)> playerUI;
};
#endif // !UIM
