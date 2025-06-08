#ifndef GMAG
#define GMAG

#include "weapon.h"
#include "data_base.h"
#include "event_system.h"
#include "map_manager.h"
#include "const.h"

class ItemManager {
public:
	ItemManager();
	~ItemManager() = default;
	Gun* const GetGunByIndex(int index) const;
	Gun* const GetGunById(int id) const;
	Itemble* const GetItemble(int id) const;
private:
	void ResetGuns();
	void SaveGun() const;
	void CreateImprovements();
	void CreateGuns();
	void CreateItems();
	void CreateTraveler();
	const Animator<sf::Texture*> CreateAnimator(int gun_index) const;
	const Animation<sf::Texture*> CreateAnimation(std::vector<sf::Texture>* frames, float duration) const;

	std::map<int, Itemble*> itemble_;
	std::vector<std::unique_ptr<Improve>> improvements_;
	std::vector<std::unique_ptr<Item>> items_;
	std::vector<std::unique_ptr<Gun>> guns_;
	std::vector<std::unique_ptr<Item>> traveler_item_;
};

#endif // !GMAG
