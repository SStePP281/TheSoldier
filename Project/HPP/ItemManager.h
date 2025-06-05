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
	Gun* GetGunByIndex(int index);
	Gun* GetGunById(int id);
	Itemble* GetItemble(int id);
private:
	void ResetGuns();
	void SaveGun();
	void CreateImprovements();
	void CreateGuns();
	void CreateItems();
	void CreateTraveler();
	Animator<sf::Texture*> CreateAnimator(int gun_index);
	Animation<sf::Texture*> CreateAnimation(std::vector<sf::Texture>* frames, float duration);

	std::map<int, Itemble*> itemble;
	std::vector<std::unique_ptr<Improve>> improvements;
	std::vector<std::unique_ptr<Item>> items;
	std::vector<std::unique_ptr<Gun>> guns;
	std::vector<std::unique_ptr<Item>> traveler_item;
};

#endif // !GMAG
