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
	void DrawPlayerUI(Player* player);
	void InitDialog(std::map<int, std::wstring, std::greater<int>>& variants,
		const std::wstring& npcName);
	void InitResetMenu();
	void InitStartMenu();
	void InitGameMenu();
	void InitSetting();
	void InitQuest(Quest* quest, Player* player);
	void InitTrade(const std::map<int, Itemble*>& variants, Player* player);
	void InitMechanic(Player* player, Gun* choose);
	void InitChanger(int coef, Player* player);
	void InitInvent(const std::map<Itemble*, int>& items, Itemble* chose, Player* player);
	void DeleteNow();
	void DrawNow();
	int CheckButton();
private:
	std::wstring SplitText(std::wstring text, int maxLen, int textSize);
	std::wstring ToMax(std::wstring str, float maxW, float textSize);

	sf::RenderWindow* window;
	sf::Sprite background;
	int choseBut;
	std::vector<Button> buttons;
	int keyButton;
	std::function<void(Player* player)> playerUI;
};
#endif // !UIM
