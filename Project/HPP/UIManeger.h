#pragma once
#ifndef UIM
#define UIM

#include "Player.h"
#include "EventSystem.h"
#include "DataBase.h"
#include "sfmlExtantion.h"
#include "Weapon.h"
#include "CONST.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>
#include <map>

class UIManager
{
public:
	UIManager(sf::RenderWindow* _window);
	void drawPlayerUI(Player* player);
	void initDialog(std::map<int, std::wstring, std::greater<int>>& variants,
		const std::wstring& npcName);
	void initResetMenu();
	void initStartMenu();
	void initGameMenu();
	void initSetting();
	void initQuest(Quest* quest, Player* player);
	void initTrade(const std::map<int, Itemble*>& variants, Player* player);
	void initMechanic(Player* player, Gun* choose);
	void initChanger(int coef, Player* player);
	void initInvent(const std::map<Itemble*, int>& items, Itemble* chose, Player* player);
	void deleteNow();
	void drawNow();
	int checkButton();
private:
	std::wstring splitText(std::wstring text, int maxLen, int textSize);
	std::wstring toMax(std::wstring str, float maxW, float textSize);
	sf::RenderWindow* window;
	sf::Sprite background;
	int choseBut;
	std::vector<Button> buttons;
	int keyButton;
	std::function<void(Player* player)> playerUI;
};
#endif // !UIM
