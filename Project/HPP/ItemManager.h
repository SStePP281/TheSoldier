#pragma once
#ifndef GMAG
#define GMAG

#include "Weapon.h"
#include "DataBase.h"
#include "EventSystem.h"
#include "MapManager.h"
#include "CONST.h"

class ItemManager
{
public:
	ItemManager();
	~ItemManager() = default;
	Gun* getGunByIndex(int index);
	Gun* getGunById(int id);
	Itemble* getItemble(int id);
private:
	std::map<int, Itemble*> itemble;
	std::vector<std::unique_ptr<Improve>> improvements;
	std::vector<std::unique_ptr<Item>> items;
	std::vector<std::unique_ptr<Gun>> guns;
	std::vector<std::unique_ptr<Item>> travelItem;

	void resetGuns();
	void saveGun();
	void createImprovements();
	void createGuns();
	void createItems();
	void createTraveler();
	Animator<sf::Texture*> createAnimator(int gunIndex);
	Animation<sf::Texture*> createAnimation(std::vector<sf::Texture>* frames, float duration);
};

#endif // !GMAG

