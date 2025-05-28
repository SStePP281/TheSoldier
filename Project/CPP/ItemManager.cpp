#include "ItemManager.h"

ItemManager::ItemManager()
{
	createImprovements();
	createItems();
	createGuns();
	createTraveler();

	auto& event = EventSystem::getInstance();
	event.subscribe<int>("SAVE", [=](const int& NON) {saveGun();});

	event.subscribe<int>("RESET_GAME", [=](const int& NON) { resetGuns(); });

	event.subscribe<int>("WIN_GAME", [=](const int& NON) { resetGuns(); });
}

void ItemManager::resetGuns()
{
	for (int i = 0; i < guns.size(); i++)
	{
		guns[i]->deleteImprove(ImproveType::Damage);
		guns[i]->deleteImprove(ImproveType::Spread);
		guns[i]->deleteImprove(ImproveType::Magazin);
		guns[i]->nowCount = gunsDef[i].maxCount;
		guns[i]->maxCount = gunsDef[i].maxCount;
		guns[i]->damage = gunsDef[i].damage;
		guns[i]->maxRad = MAX_RAD;
		guns[i]->nowRad = MIN_RAD;
		guns[i]->maxImpRad = MAX_RAD;
		guns[i]->upgradeCount = 0;
	}
}

void ItemManager::createImprovements()
{
	for (int i = 0; i < improveDefs.size(); i++)
	{
		improvements.push_back(std::make_unique<Improve>(Improve(improveDefs[i])));
		itemble[improveDefs[i].id] = improvements.back().get();
	}
}

void  ItemManager::createItems()
{
	for (int i = 0; i < itemsDefs.size(); i++)
	{
		items.push_back(std::make_unique<Item>(Item(itemsDefs[i])));
		itemble[itemsDefs[i].id] = items.back().get();
	}
}

void ItemManager::createTraveler()
{
	for (int i = 0; i < travelerDefs.size(); i++)
	{
		items.push_back(std::make_unique<Item>(Item(travelerDefs[i])));
		itemble[travelerDefs[i].id] = items.back().get();
		items.back()->id = travelerDefs[0].id;
	}
}

void  ItemManager::createGuns()
{
	auto& data = Data::getInstance();
	auto gunsData = data.getGunData();

	for (int i = 0; i < gunsDef.size(); i++)
	{
		GunDef def = gunsDef[i];
		def.nowCount = gunsData[i].nowCount;
		def.maxCount = gunsData[i].nowMaxCount;
		def.damage = gunsData[i].nowDamage;

		guns.push_back(std::make_unique<Gun>(Gun(def, i > 1, i)));
		guns.back()->setAnimator(std::move(createAnimator(i)));
		guns.back()->maxRad = gunsData[i].nowMaxRad;
		guns.back()->upgradeCount = gunsData[i].upgradeCount;

		itemble[def.id] = guns.back().get();

		for (auto im : gunsData[i].improveId)
		{
			guns.back()->trySetImprove(improvements[im].get());
		}
	}
}

Animator<sf::Texture*> ItemManager::createAnimator(int gunIndex)
{
	return Animator<sf::Texture*>(&Resources::gunsBaseText[gunIndex],
		{ createAnimation(&Resources::gunsFireAnim[gunIndex], gunsDef[gunIndex].shutTime),
		createAnimation(&Resources::gunsResetAnim[gunIndex], gunsDef[gunIndex].resetTime) });
}

Animation<sf::Texture*> ItemManager::createAnimation(std::vector<sf::Texture>* frames, float duration)
{
	Animation<sf::Texture*> anim;
	int count = (int)frames->size();

	for (int j = 0; j < count; ++j)
	{
		anim.setKeyframe((j + 1) / (float)count * duration, &(*frames)[j]);
	}

	if (count > 0)
	{
		anim.setKeyframe(duration, &(*frames)[count - 1]);
	}

	return anim;
}

Gun* ItemManager::getGunByIndex(int index) { return index >= 0 && index < guns.size() ?
	guns[index].get(): throw "Invalid index"; }

Gun* ItemManager::getGunById(int id) { return dynamic_cast<Gun*>(itemble[id]); }

Itemble* ItemManager::getItemble(int id) { return itemble[id]; }

void ItemManager::saveGun()
{
	std::vector<GunData> defs;
	defs.reserve(guns.size());

	for (const auto& gun : guns)
	{
		defs.push_back(gun->getGunData());
	}

	auto& data = Data::getInstance();
	data.saveGunData(defs);
}