#pragma once
#ifndef INVENT
#define INVENT

#include "UIManeger.h"
#include "ItemManager.h"
#include "DataBase.h"
#include "RenderState.h"

class Inventory
{
public:
	Inventory(sf::RenderWindow* _window, Player* _player, UIManager* _uiManager);

	Item* takeMaxHeal();
	void takeItem(Itemble*, int cnt = 1);
	void useItem(Itemble*, int cnt = 1);
	void useInvent();
	void update();
	void drawInvent();
private:
	std::vector<std::pair<int, int>> convert2save();
	void initInv();
	void checkChose();
	void selectedItem();
	void selectItemById();
	void useSelectedItem(Item* item);
	void useSelectedGun(Gun* gun);
	void useSelectedImprove(Improve* improve);

	bool isOpen;
	Player* player;
	int nowKey;
	RenderState invetState;
	Itemble* choose;
	std::map<Itemble*, int> items;
	UIManager* uiManager;
	sf::RenderWindow* window;
};

#endif // !INVENT