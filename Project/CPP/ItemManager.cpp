#include "ItemManager.h"

ItemManager::ItemManager()
{
	CreateImprovements();
	CreateItems();
	CreateGuns();
	CreateTraveler();

	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int& NON) {SaveGun();});

	event.Subscribe<int>("RESET_GAME", [=](const int& NON) { ResetGuns(); });

	event.Subscribe<int>("WIN_GAME", [=](const int& NON) { ResetGuns(); });
}

void ItemManager::ResetGuns()
{
	for (int i = 0; i < guns.size(); i++)
	{
		guns[i]->DeleteImprove(ImproveType::Damage);
		guns[i]->DeleteImprove(ImproveType::Spread);
		guns[i]->DeleteImprove(ImproveType::Magazin);
		guns[i]->now_count = gunsDef[i].maxCount;
		guns[i]->max_count = gunsDef[i].maxCount;
		guns[i]->damage = gunsDef[i].damage;
		guns[i]->max_rad = kMaxRad;
		guns[i]->now_rad = kMinRad;
		guns[i]->max_imp_rad = kMaxRad;
		guns[i]->upgrade_count = 0;
	}
}

void ItemManager::CreateImprovements()
{
	for (int i = 0; i < improveDefs.size(); i++)
	{
		improvements.push_back(std::make_unique<Improve>(Improve(improveDefs[i])));
		itemble[improveDefs[i].id] = improvements.back().get();
	}
}

void  ItemManager::CreateItems()
{
	for (int i = 0; i < itemsDefs.size(); i++)
	{
		items.push_back(std::make_unique<Item>(Item(itemsDefs[i])));
		itemble[itemsDefs[i].id] = items.back().get();
	}
}

void ItemManager::CreateTraveler()
{
	for (int i = 0; i < travelerDefs.size(); i++)
	{
		items.push_back(std::make_unique<Item>(Item(travelerDefs[i])));
		itemble[travelerDefs[i].id] = items.back().get();
		items.back()->id = travelerDefs[0].id;
	}
}

void  ItemManager::CreateGuns()
{
	auto& data = Data::GetInstance();
	auto guns_data = data.GetGunData();

	for (int i = 0; i < gunsDef.size(); i++)
	{
		GunDef def = gunsDef[i];
		def.nowCount = guns_data[i].nowCount;
		def.maxCount = guns_data[i].nowMaxCount;
		def.damage = guns_data[i].nowDamage;

		guns.push_back(std::make_unique<Gun>(Gun(def, i > 1, i)));
		guns.back()->SetAnimator(std::move(CreateAnimator(i)));
		guns.back()->max_rad = guns_data[i].nowMaxRad;
		guns.back()->upgrade_count = guns_data[i].upgradeCount;

		itemble[def.id] = guns.back().get();

		for (auto im : guns_data[i].improveId)
		{
			guns.back()->TrySetImprove(improvements[im].get());
		}
	}
}

Animator<sf::Texture*> ItemManager::CreateAnimator(int gun_index)
{
	return Animator<sf::Texture*>(&Resources::guns_base_text[gun_index],
		{ CreateAnimation(&Resources::guns_fire_anim[gun_index], gunsDef[gun_index].shutTime),
		CreateAnimation(&Resources::guns_reset_anim[gun_index], gunsDef[gun_index].resetTime) });
}

Animation<sf::Texture*> ItemManager::CreateAnimation(std::vector<sf::Texture>* frames, float duration)
{
	Animation<sf::Texture*> animation;
	int count = (int)frames->size();

	for (int j = 0; j < count; ++j)
	{
		animation.SetKeyframe((j + 1) / (float)count * duration, &(*frames)[j]);
	}

	if (count > 0)
	{
		animation.SetKeyframe(duration, &(*frames)[count - 1]);
	}

	return animation;
}

Gun* ItemManager::GetGunByIndex(int index) { return index >= 0 && index < guns.size() ?
	guns[index].get(): throw "INVALID INDEX!"; }

Gun* ItemManager::GetGunById(int id) { return dynamic_cast<Gun*>(itemble[id]); }

Itemble* ItemManager::GetItemble(int id) { return itemble[id]; }

void ItemManager::SaveGun()
{
	std::vector<GunData> defs;
	defs.reserve(guns.size());

	for (const auto& gun : guns)
	{
		defs.push_back(gun->GetGunData());
	}

	auto& data = Data::GetInstance();
	data.SaveGunData(defs);
}