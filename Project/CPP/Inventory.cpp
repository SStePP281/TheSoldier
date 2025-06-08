#include "inventory.h"

Inventory::Inventory(sf::RenderWindow* window, Player* const player, UIManager* const uiManager) :
	window_{ window }, ui_manager_{ uiManager }, player_{ player }, is_open_{ false },
	choose_{nullptr}, now_key_{0}, invetory_state_{ [=](float delta_time) {Update();}, [=]() {DrawInvent();} } {	
	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int NON) { Save(); });

	event.Subscribe<int>("RESET_GAME", [=](const int NON) { items_.clear(); });

	event.Subscribe<int>("WIN_GAME", [&](const int NON) { items_.clear(); });
}

Item* const Inventory::TakeMaxHeal() const {
	Item* max_heal = nullptr;

	for (const auto& [item, count] : items_) {
		auto heal_item = dynamic_cast<Item*>(item);
		if (heal_item && heal_item->GetType() == ItemType::Heal) {
			if (!max_heal || heal_item->GetId() > max_heal->GetId()) {
				max_heal = heal_item;
			}
		}
	}

	return max_heal;
}

void Inventory::TakeItem(Itemble* const itemble, int count) {
	if (itemble) { 
		items_[itemble] += count; 
	}
}

void Inventory::UseItem(Itemble* const itemble, int count) {
	if (!itemble) return;

	if (items_[itemble] - count >= 0) {
		items_[itemble] -= count;

		if (items_[itemble] == 0) {
			items_.erase(itemble);
		}
	}
}

void Inventory::Save() const {
	std::vector<std::pair<int, int>> inv;

	for (const auto& it : items_) {
		inv.push_back({ it.first->GetId(), it.second});
	}

	auto& data = Data::GetInstance();
	data.SaveInvent(inv);
}

void Inventory::UseInvent() { 
	ui_manager_->DeleteNow();
	is_open_ = !is_open_;
	window_->setMouseCursorVisible(is_open_);

	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", is_open_ ? &invetory_state_ : nullptr);

	if (!is_open_) {
		choose_ = nullptr;
	} else {
		InitInv();
	}
}

void Inventory::InitInv() {
	ui_manager_->DeleteNow();
	ui_manager_->InitInvent(items_, choose_, player_);
}

void Inventory::CheckChose() {
	if (now_key_ >= 100) {
		SelectedItem();
	} else {
		SelectItemById();
	}
}

void Inventory::SelectedItem() {
	if (auto item = dynamic_cast<Item*>(choose_); item) { UseSelectedItem(item); }
	else if (auto gun = dynamic_cast<Gun*>(choose_); gun) { UseSelectedGun(gun); }
	else if (auto improve = dynamic_cast<Improve*>(choose_); improve) { UseSelectedImprove(improve); }
	
	choose_ = nullptr;
	InitInv();
}

void Inventory::SelectItemById() {
	for (const auto& [item, count] : items_) {
		if (item->GetId() == now_key_) {
			choose_ = item;
			InitInv();
			break;
		}
	}
}

void Inventory::UseSelectedItem(Item* const item) {
	item->UseItem(player_);
	UseItem(choose_);
}

void Inventory::UseSelectedGun(Gun* const gun) {
	if (now_key_ == 100) {
		if (player_->GetGun(2) != gun) { player_->SetGun(gun, 1); }
	} else if (now_key_ == 101) {
		if (player_->GetGun(1) != gun) { player_->SetGun(gun, 2); }
	} else {
		int i = 102;
		for (const auto& imp : gun->GetAllImprovments()) {
			if (i == now_key_) {
				items_[dynamic_cast<Itemble*>(gun->DeleteImprove(imp.first))] += 1;
				break;
			}

			i++;
		}
	}
}

void Inventory::UseSelectedImprove(Improve* const improve) {
	Gun* target_gun = nullptr;

	if (now_key_ == 100) { target_gun = player_->GetGun(1); }
	else if (now_key_ == 101) { target_gun = player_->GetGun(2); }

	if (target_gun) {
		UseItem(choose_);
		if (auto removed_improve = target_gun->TrySetImprove(improve); removed_improve) {
			items_[dynamic_cast<Itemble*>(removed_improve)] += 1;
		}
	}
}

void Inventory::Update() {
	if (!window_->hasFocus()) return;

	static bool is_mouse_down = false;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !is_mouse_down) {
		is_mouse_down = true;

		int key = ui_manager_->CheckButton();
		if (key != -1) {
			now_key_ = key;
			CheckChose();
		}
	} else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		is_mouse_down = false;
	}
}

void Inventory::DrawInvent() {
	ui_manager_->DrawNow();
}
