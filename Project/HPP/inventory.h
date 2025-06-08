#ifndef INVENT
#define INVENT

#include "ui_maneger.h"
#include "item_manager.h"
#include "data_base.h"
#include "render_state.h"

class Inventory {
public:
	Inventory(sf::RenderWindow* window, Player* const player, UIManager* const ui_manager);

	Item* const TakeMaxHeal() const;
	void TakeItem(Itemble* const itemble, int count = 1);
	void UseItem(Itemble* const itemble, int count = 1);
	void UseInvent();
	void Update();
	void DrawInvent();
private:
	void Save() const;
	void InitInv();
	void CheckChose();
	void SelectedItem();
	void SelectItemById();
	void UseSelectedItem(Item* const item);
	void UseSelectedGun(Gun* const gun);
	void UseSelectedImprove(Improve* const improve);

	bool is_open_;
	int now_key_;
	Player* const player_;
	RenderState invetory_state_;
	Itemble* choose_;
	std::map<Itemble*, int> items_;
	UIManager* const ui_manager_;
	sf::RenderWindow* window_;
};

#endif // !INVENT
