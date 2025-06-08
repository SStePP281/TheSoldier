#include "weapon.h"
#include "player.h"

Weapon::Weapon(float time_betewen, float max_dist) : time_between_{ time_betewen }, 
now_time_{ time_betewen }, max_dist_{ max_dist } {}

void Weapon::Update(float delta_time) {
	weapon_animator_.Update(delta_time);
	if (now_time_ >= time_between_) return;
	now_time_ += delta_time;
}

void Weapon::SetAnimator(const Animator<sf::Texture*>&& animator) { weapon_animator_ = animator; }

const float Weapon::GetMaxDistance() const { return max_dist_; }

void Weapon::DrawWeapon(sf::RenderTarget* window, const sf::Vector2f& delta_shake) const {
	sf::Texture* texture = weapon_animator_.Get();

	if (texture) {
		sf::Sprite weapon{ *texture };
		weapon.setOrigin(texture->getSize().x / 2.0f, (float)texture->getSize().y);
		weapon.scale(2.5f, 2.5f);
		weapon.setPosition(kScreenWight / 2.0f - 20 + delta_shake.x, kScreenHeight + delta_shake.y + 10);
		window->draw(weapon);
	}
}

const bool Weapon::IsCanUsed() const {
	return now_time_ >= time_between_;
}

void Weapon::StartAnimation(int number) {
	now_time_ = 0.0f;
	weapon_animator_.SetAnimation(number);
}

Itemble::Itemble(const std::wstring& name, const std::wstring& disc, int cost, int texture_id) :
	name_{ name }, disc_{ disc }, cost_{ cost }, id_{ texture_id } {}

const std::wstring& Itemble::GetName() const { return name_; }

const std::wstring& Itemble::GetDiscription() const { return disc_; }

const int Itemble::GetCost() const { return cost_; }

const int Itemble::GetId() const { return id_; }

Improve::Improve(const ImproveDef& improve_def) :
	Itemble(improve_def.name, improve_def.disc, improve_def.cost, improve_def.id), type_{ improve_def.type } {
	if (type_ == ImproveType::Damage) {
		SetGetFunc([=](Gun* gun) {gun->SetDamage((int)round(gun->GetDamage() * improve_def.effect));});

		SetDelFunc([=](Gun* gun) {gun->SetDamage((int)round(gun->GetDamage() / improve_def.effect));});
	} else if (type_ == ImproveType::Spread) {
		SetGetFunc([=](Gun* gun) {gun->SetMaxImpRadius(round(gun->GetMaxImpRadius() / improve_def.effect));});

		SetDelFunc([=](Gun* gun) {gun->SetMaxImpRadius(round(gun->GetMaxImpRadius() * improve_def.effect));});
	} else if (type_ == ImproveType::Magazin) {
		SetGetFunc([=](Gun* gun) {gun->SetMaxPatronsCount((int)round(gun->GetMaxPatronsCount() * improve_def.effect));});

		SetDelFunc([=](Gun* gun) {gun->SetMaxPatronsCount((int)round(gun->GetMaxPatronsCount() / improve_def.effect));});
	}
}

void Improve::SetGetFunc(std::function<void(Gun* gun)>&& set_effect) { get_improve_ = set_effect; }

void Improve::SetDelFunc(std::function<void(Gun* gun)>&& del_effect) { delete_improve_ = del_effect; }

void Improve::TakeImprove(Gun* gun) const { get_improve_(gun); }

void Improve::PutOffImprove(Gun* gun) const { delete_improve_(gun); }

const ImproveType& Improve::GetType() const { return type_; }

Item::Item(const ItemsDef& item_def) : Itemble(item_def.name, item_def.disc, item_def.cost, item_def.id),
type_{ item_def.type }, max_using_{ item_def.max_using } {
	if (item_def.type == ItemType::Heal) {
		SetFunc([=](Player* pl) {pl->GetEnemy()->SetHealpoint(pl->GetEnemy()->GetHealpoint() + item_def.effect);});
	} else if (item_def.type == ItemType::MaxHeal) {
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f) {
				pl->GetEnemy()->SetMaxHealPoint(pl->GetEnemy()->GetMaxHealpoint() + item_def.effect);
			}
			else {
				pl->GetEnemy()->SetMaxHealPoint(pl->GetEnemy()->GetMaxHealpoint() - item_def.effect / 2);
			}
		});
	} else if (item_def.type == ItemType::MaxEnergy) {
		SetFunc([=](Player* pl) {
			if (Random::BitRandom() > 0.2f) {
				pl->SetMaxEnergy(pl->GetMaxEnergy() + item_def.effect);
			}
			else {
				pl->SetMaxEnergy(pl->GetMaxEnergy() - item_def.effect / 2);
			}
		});
	} else if (item_def.type == ItemType::Armor) {
		SetFunc([=](Player* pl) {pl->SetDefence((float)item_def.effect); pl->SetMaxStrenght((float)item_def.max_using);});
	} else if (item_def.type == ItemType::Patrons) {
		SetFunc([=](Player* pl) {pl->SetPatrons(pl->GetPatronsCount() + item_def.effect);});
	}
}

void Item::SetFunc(std::function<void(Player* sprite)>&& _use_func) { use_func_ = _use_func; }

void Item::UseItem(Player* sprite) const { use_func_(sprite); }

const ItemType& Item::GetType() const { return type_; }

Gun::Gun(const GunDef& gun_def, bool is_reset, int gun_id) : Weapon(gun_def.shut_time, gun_def.max_dist),
Itemble(gun_def.name, gun_def.disc, gun_def.cost, gun_def.id), damage_{ gun_def.damage }, upgrade_count_{ 0 },
max_count_{ gun_def.max_count }, now_count_{ gun_def.now_count },now_time_betwen_reset_{ gun_def.reset_time }, 
time_betwen_reset_{ gun_def.reset_time }, is_reset_{ is_reset }, gun_id_{ gun_id }, max_rad_{kMaxRad}, now_rad_{kMinRad},
max_imp_rad_{kMaxRad} {}

void Gun::Update(float delta_time) {
	Weapon::Update(delta_time);

	if (is_reset_) {
		if (now_time_betwen_reset_ >= time_betwen_reset_) return;
		now_time_betwen_reset_ += delta_time;
	}
}

void Gun::UpdateRad(bool is_run, float delta_time) {
	if (is_run) {
		now_rad_ = std::min(now_rad_ + kMaxRad * delta_time * 2, max_imp_rad_);
	} else {
		now_rad_ = std::max(now_rad_ - kMaxRad * delta_time * 2, 1.0f);
	}
}

const int Gun::ResetPatron(int count) {
	if (!is_reset_) return count;
	
	if (now_time_betwen_reset_ < time_betwen_reset_ && now_count_ >= max_count_ && !IsCanUsed()) return count;
	
	auto delta = max_count_ - now_count_;

	if (delta <= 0 || count == 0) {
		return count;
	} else if (delta < count) {
		count -= delta;
		now_count_ = max_count_;
	} else if (count > 0) {
		now_count_ += count;
		count = 0;
	}

	now_time_betwen_reset_ = 0;
	SoundManager::PlaySounds(Resources::guns_reset_sound[gun_id_]);
	StartAnimation(1);

	return count;
}

void Gun::Ussing(Enemy* enemy, float dist) {
	if (now_count_ == 0 && is_reset_) {
		SoundManager::PlaySounds(Resources::gun_cant_shout_sound);
		return;
	} else if (IsCanUsed() && (now_time_betwen_reset_ >= time_betwen_reset_ || !is_reset_)) {
		if (enemy) {
			if (Random::BitRandom() > (now_rad_ - 0.05f) / kMaxRad - 0.35f || now_rad_ == kMinRad) {
				enemy->TakeDamage((float)damage_ * (dist < max_dist_ ? 1 : 0));
			}
		}

		now_count_--;
		SoundManager::PlaySounds(Resources::guns_shut_sound[gun_id_]);
		StartAnimation(0);
	}
}

void Gun::ResetGunData() {
	for (int i = 0; i < 3; i++) {
		DeleteImprove(ImproveType(i));
	}
	
	now_count_ = guns_def[gun_id_].max_count;
	max_count_ = guns_def[gun_id_].max_count;
	damage_ = guns_def[gun_id_].damage;
	max_rad_ = kMaxRad;
	now_rad_ = kMinRad;
	max_imp_rad_ = kMaxRad;
	upgrade_count_ = 0;
}

const bool Gun::IsResetable() const { return is_reset_; }

void Gun::SetDamage(int _damage) { damage_ = _damage; }

const int Gun::GetDamage() const { return damage_; }

void Gun::SetMaxPatronsCount(int count) { 
	max_count_ = count;
	now_count_ = std::min(max_count_, now_count_);
}

const int Gun::GetMaxPatronsCount() const { return max_count_; }

const int Gun::GetNowPatronsCount() const { return now_count_; }

void Gun::SetMaxRadius(int max_radius) { max_rad_ = max_radius; }

const int Gun::GetMaxRadius() const { return max_rad_; }

void Gun::SetMaxImpRadius(float max_imp_radius) {
	max_imp_rad_ = max_imp_radius;
	now_rad_ = std::min(max_imp_rad_, now_rad_);
}

const float Gun::GetMaxImpRadius() const {return max_imp_rad_;}

const float Gun::GetNowRadius() const { return now_rad_; }

void Gun::TakeOneUpgrade() { upgrade_count_++; }

void Gun::SetUpgradeCount(int count) { upgrade_count_ = count; }

const int Gun::GetUpgradeCount() const { return upgrade_count_; }

const std::map<ImproveType, Improve*>& Gun::GetAllImprovments() const { return improvement_; }

Improve* const Gun::TrySetImprove(Improve* const improve) {
	if (!improve) return nullptr;

	auto temp = improvement_[improve->GetType()];
	if (temp) { temp->PutOffImprove(this); }

	improvement_[improve->GetType()] = improve;
	improve->TakeImprove(this);

	return temp;
}

Improve* const Gun::DeleteImprove(ImproveType type) {
	auto temp = improvement_[type];

	if (temp) {
		temp->PutOffImprove(this);
		improvement_.erase(type);
	}

	return temp;
}

const GunData Gun::GetGunData() {
	GunData data;

	for (const auto& im : improvement_) {
		if (im.second) {
			data.improve_id.push_back(im.second->GetId());
		}
	}

	data.id = id_;
	data.upgrade_count = upgrade_count_;
	data.now_count = now_count_;

	Improve* imp;
	imp = DeleteImprove(ImproveType::Magazin);
	data.now_max_ount = max_count_;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Damage);
	data.now_damage = damage_;
	TrySetImprove(imp);

	imp = DeleteImprove(ImproveType::Spread);
	data.now_max_rad = max_rad_;
	TrySetImprove(imp);

	return data;
}
