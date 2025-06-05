#ifndef INVENT
#define INVENT

#include "UIManeger.h"
#include "ItemManager.h"
#include "DataBase.h"
#include "RenderState.h"

class Inventory
{
public:
	Inventory(sf::RenderWindow* window, Player* player, UIManager* ui_manager);

	Item* TakeMaxHeal();
	void TakeItem(Itemble* itemble, int count = 1);
	void UseItem(Itemble* itemble, int count = 1);
	void UseInvent();
	void Update();
	void DrawInvent();
private:
	void Save();
	void InitInv();
	void CheckChose();
	void SelectedItem();
	void SelectItemById();
	void UseSelectedItem(Item* item);
	void UseSelectedGun(Gun* gun);
	void UseSelectedImprove(Improve* improve);

	bool is_open;
	Player* player;
	int now_key;
	RenderState invetory_state;
	Itemble* choose;
	std::map<Itemble*, int> items;
	UIManager* ui_manager;
	sf::RenderWindow* window;
};

#endif // !INVENT
