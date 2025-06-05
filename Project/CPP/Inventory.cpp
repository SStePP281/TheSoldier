#include "Inventory.h"

Inventory::Inventory(sf::RenderWindow* window, Player* player, UIManager* uiManager) :
	window{ window }, ui_manager{ uiManager }, player{ player }, is_open{ false },
	choose{nullptr}, now_key{0}, invetory_state{ [=](float delta_time) {Update();}, [=]() {DrawInvent();} }
{	
	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int NON) { Save(); });

	event.Subscribe<int>("RESET_GAME", [=](const int NON) { items.clear(); });

	event.Subscribe<int>("WIN_GAME", [&](const int NON) { items.clear(); });
}

Item* Inventory::TakeMaxHeal()
{
	Item* max_heal = nullptr;

	for (const auto& [item, count] : items)
	{
		auto heal_item = dynamic_cast<Item*>(item);
		if (heal_item && heal_item->type == ItemType::Heal)
		{
			if (!max_heal || heal_item->id > max_heal->id)
			{
				max_heal = heal_item;
			}
		}
	}

	return max_heal;
}

void Inventory::TakeItem(Itemble* itemble, int count)
{
	if (itemble) 
	{ 
		items[itemble] += count; 
	}
}

void Inventory::UseItem(Itemble* itemble, int count)
{
	if (!itemble) return;

	if (items[itemble] - count >= 0)
	{
		items[itemble] -= count;

		if (items[itemble] == 0)
		{
			items.erase(itemble);
		}
	}
}

void Inventory::Save()
{
	std::vector<std::pair<int, int>> inv;

	for (auto it : items)
	{
		inv.push_back({ it.first->id, it.second });
	}

	auto& data = Data::GetInstance();
	data.SaveInvent(inv);
}

void Inventory::UseInvent() 
{ 
	ui_manager->DeleteNow();
	is_open = !is_open;
	window->setMouseCursorVisible(is_open);

	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", is_open ? &invetory_state : nullptr);

	if (!is_open)
	{
		choose = nullptr;
	}
	else
	{
		InitInv();
	}
}

void Inventory::InitInv()
{
	ui_manager->DeleteNow();
	ui_manager->InitInvent(items, choose, player);
}

void Inventory::CheckChose()
{
	if (now_key >= 100)
	{
		SelectedItem();
	}
	else
	{
		SelectItemById();
	}
}

void Inventory::SelectedItem()
{
	if (auto item = dynamic_cast<Item*>(choose); item) 
	{ UseSelectedItem(item); }
	else if (auto gun = dynamic_cast<Gun*>(choose); gun) 
	{ UseSelectedGun(gun); }
	else if (auto improve = dynamic_cast<Improve*>(choose); improve) 
	{ UseSelectedImprove(improve); }
	
	choose = nullptr;
	InitInv();
}

void Inventory::SelectItemById()
{
	for (const auto& [item, count] : items)
	{
		if (item->id == now_key)
		{
			choose = item;
			InitInv();
			break;
		}
	}
}

void Inventory::UseSelectedItem(Item* item)
{
	item->use_func(player);
	UseItem(choose);
}

void Inventory::UseSelectedGun(Gun* gun)
{
	if (now_key == 100)
	{
		if (player->GetGun(2) != gun) { player->SetGun(gun, 1); }
	}
	else if (now_key == 101)
	{
		if (player->GetGun(1) != gun) { player->SetGun(gun, 2); }
	}
	else
	{
		int i = 102;
		for (auto it : gun->improvement)
		{
			if (i == now_key)
			{
				items[dynamic_cast<Itemble*>(gun->DeleteImprove(it.first))] += 1;
				break;
			}

			i++;
		}
	}
}

void Inventory::UseSelectedImprove(Improve* improve)
{
	Gun* target_gun = nullptr;

	if (now_key == 100) { target_gun = player->GetGun(1); }
	else if (now_key == 101) { target_gun = player->GetGun(2); }

	if (target_gun)
	{
		UseItem(choose);
		if (auto removed_improve = target_gun->TrySetImprove(improve); removed_improve)
		{
			items[dynamic_cast<Itemble*>(removed_improve)] += 1;
		}
	}
}

void Inventory::Update()
{
	if (!window->hasFocus()) return;

	static bool is_mouse_down = false;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !is_mouse_down)
	{
		is_mouse_down = true;

		int key = ui_manager->CheckButton();
		if (key != -1)
		{
			now_key = key;
			CheckChose();
		}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		is_mouse_down = false;
	}
}

void Inventory::DrawInvent()
{
	ui_manager->DrawNow();
}
