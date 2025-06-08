#include "item_manager.h"

ItemManager::ItemManager() {
	CreateImprovements();
	CreateItems();
	CreateGuns();
	CreateTraveler();

	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int& NON) {SaveGun();});

	event.Subscribe<int>("RESET_GAME", [=](const int& NON) { ResetGuns(); });

	event.Subscribe<int>("WIN_GAME", [=](const int& NON) { ResetGuns(); });
}

void ItemManager::ResetGuns() {
	for (int i = 0; i < guns_.size(); i++) {
		guns_[i]->ResetGunData();
	}
}

void ItemManager::CreateImprovements() {
	for (int i = 0; i < improve_defs.size(); i++) {
		improvements_.push_back(std::make_unique<Improve>(Improve(improve_defs[i])));
		itemble_[improve_defs[i].id] = improvements_.back().get();
	}
}

void  ItemManager::CreateItems() {
	for (int i = 0; i < items_defs.size(); i++) {
		items_.push_back(std::make_unique<Item>(Item(items_defs[i])));
		itemble_[items_defs[i].id] = items_.back().get();
	}
}

void ItemManager::CreateTraveler() {
	for (int i = 0; i < traveler_defs.size(); i++) {
		items_.push_back(std::make_unique<Item>(Item(traveler_defs[i])));
		itemble_[traveler_defs[i].id] = items_.back().get();
	}
}

void  ItemManager::CreateGuns() {
	auto& data = Data::GetInstance();
	auto guns_data = data.GetGunData();

	for (int i = 0; i < guns_def.size(); i++) {
		GunDef def = guns_def[i];
		def.now_count = guns_data[i].now_count;
		def.max_count = guns_data[i].now_max_ount;
		def.damage = guns_data[i].now_damage;

		guns_.push_back(std::make_unique<Gun>(Gun(def, i > 1, i)));
		guns_.back()->SetAnimator(std::move(CreateAnimator(i)));
		guns_.back()->SetMaxRadius(guns_data[i].now_max_rad);
		guns_.back()->SetUpgradeCount(guns_data[i].upgrade_count);

		itemble_[def.id] = guns_.back().get();

		for (const auto& im : guns_data[i].improve_id) {
			guns_.back()->TrySetImprove(improvements_[im].get());
		}
	}
}

const Animator<sf::Texture*> ItemManager::CreateAnimator(int gun_index) const {
	return Animator<sf::Texture*>(&Resources::guns_base_text[gun_index],
		{ CreateAnimation(&Resources::guns_fire_anim[gun_index], guns_def[gun_index].shut_time),
		CreateAnimation(&Resources::guns_reset_anim[gun_index], guns_def[gun_index].reset_time) });
}

const Animation<sf::Texture*> ItemManager::CreateAnimation(std::vector<sf::Texture>* frames, float duration) const {
	Animation<sf::Texture*> animation;
	int count = (int)frames->size();

	for (int j = 0; j < count; ++j) {
		animation.SetKeyframe((j + 1) / (float)count * duration, &(*frames)[j]);
	}

	if (count > 0) {
		animation.SetKeyframe(duration, &(*frames)[count - 1]);
	}

	return animation;
}

Gun* const ItemManager::GetGunByIndex(int index) const { return index >= 0 && index < guns_.size() ?
	guns_[index].get(): throw "INVALID INDEX!"; }

Gun* const ItemManager::GetGunById(int id) const { return dynamic_cast<Gun* const>(itemble_.at(id)); }

Itemble* const ItemManager::GetItemble(int id) const { return itemble_.at(id); }

void ItemManager::SaveGun() const {
	std::vector<GunData> defs;
	defs.reserve(guns_.size());

	for (const auto& gun : guns_) {
		defs.push_back(gun->GetGunData());
	}

	auto& data = Data::GetInstance();
	data.SaveGunData(defs);
}
