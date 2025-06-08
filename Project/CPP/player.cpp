#include "player.h"
#include "inventory.h"

Player::Player(Enemy* const sprite, const PlayerDef& def, Map* now_map) :
	now_energy_{ def.now_energy }, max_energy_ { def.max_energy },
	defence_{ def.defence}, now_strenght_{ def.now_strenght }, max_strenght_{ def.max_strenght }, 
	patrons_{ def.patrons_count }, money_{ def.money }, details_{ def.details },
	now_map_{ now_map }, enemy_{ sprite }, now_gun_{ 0 }, now_heal_{ nullptr }, kick_{ nullptr },
	inventory_{ nullptr }, pitch_{ 0 }, shake_time_{ 0 }, pos_z_{ 0 }, is_jump_{ false }, jump_flag_{ false },
	boost_speed_{ 8.0f }, now_speed_{ enemy_->GetMoveSpeed() }, mouse_speed_{1.0f} {
	enemy_->SetTextureSize(0);

	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SAVE", [=](const int NON) { auto& data = Data::GetInstance(); data.SavePlayerData(this);});

	event.Subscribe<int>("RESET_GAME", [=](const int NON) { ResetPlayer(); });

	event.Subscribe<int>("WIN_GAME", [=](const int NON) { ResetPlayer(); });
}

void Player::ResetPlayer() {
	now_energy_ = 2.0f;
	max_energy_ = 2.0f;
	defence_ = 0.0f;
	now_strenght_ = 0.0f;
	max_strenght_ = 0.0f;
	patrons_ = 200;
	money_ = 500;
	details_ = 0;
	now_heal_ = nullptr;
	enemy_->SetMaxHealPoint(100.0f);
	enemy_->SetHealpoint(100.0f);
}

const Gun* const Player::SetGun(Gun* const  gun, int position) {
	auto temp = guns_[position];
	guns_[position] = gun;
	return temp;
}

Gun* const Player::GetGun(int position) const { return guns_[position]; }

void Player::SetInventory(Inventory* const _inventory) { 
	if (inventory_ = _inventory; inventory_) { now_heal_ = inventory_->TakeMaxHeal(); }
}

void Player::UpdateMouseData(const sf::Vector2f& mouse_position, float delta_time) {
	enemy_->SetAngle(enemy_->GetAngle() + mouse_speed_ * kRotationSpeed * mouse_position.x * delta_time);

	pitch_ = std::clamp(pitch_ - mouse_position.y * delta_time * kVerticalMouseSpeed, -200.0f, 200.0f);

	guns_[now_gun_]->Update(delta_time);
	if (kick_) kick_->Update(delta_time);

	Gravity(delta_time);
}

void Player::CheckBoost(bool is_pressed, float delta_time) {
	static bool boost_flag = false;

	now_speed_ = enemy_->GetMoveSpeed();

	if (boost_flag) {
		now_energy_ = std::min(now_energy_ + delta_time, max_energy_);
		if (now_energy_ >= max_energy_) {
			boost_flag = false;
		}
		return;
	}

	if (!is_pressed) {
		now_energy_ = std::min(now_energy_ + delta_time, max_energy_);
	} else {
		if (now_energy_ > 0) {
			now_speed_ = boost_speed_;
			now_energy_ = std::max(now_energy_ - delta_time * 1.2f, 0.0f);
		} else {
			boost_flag = true;
		}
	}
}

void Player::Move(const sf::Vector2f& delta_position, float delta_time) {
	guns_[now_gun_]->UpdateRad(delta_position != sf::Vector2f(), delta_time);
	sf::Vector2f delta_move = delta_position * delta_time * now_speed_;
	enemy_->Move(now_map_, delta_move);
	RhakeCamera(delta_time, delta_position != sf::Vector2f());
}

void Player::RhakeCamera(float delta_time, bool is_run) {
	if (is_run) {
		float scale = 5.0f;
		shake_time_ += delta_time;
		shake_delta_.x = cosf(shake_time_ * now_speed_) * scale;
		shake_delta_.y = sinf(2 * shake_time_ * now_speed_) * scale;
	} else {
		shake_time_ = 0;
		if (shake_delta_ == sf::Vector2f()) return;

		shake_delta_ *= 0.9f;

		shake_delta_.x = std::abs(shake_delta_.x) < 0.0001f ? 0.0f : shake_delta_.x;
		shake_delta_.y = std::abs(shake_delta_.y) < 0.0001f ? 0.0f : shake_delta_.y;
	}
}

void Player::Gravity(float delta_time) {
	if (!is_jump_) return;
	
	if (pos_z_ < 0) {
		is_jump_ = false;
		jump_flag_ = false;
		pos_z_ = 0;
	} else {
		if (pos_z_ > 150) jump_flag_ = true;

		pos_z_ += (jump_flag_ ? -750 : 750) * delta_time;
		pos_z_ = std::max(pos_z_, -1.0f);
	}
}

void Player::Jump() { if (pos_z_ == 0) { is_jump_ = true; } }

void Player::ReloadingGun() { patrons_ = guns_[now_gun_]->ResetPatron(patrons_); }

void Player::TakeDamage(float damage) {
	if (now_strenght_ > 0) {
		damage *= (1 - defence_ / 100.f);
		now_strenght_ -= damage * 10.0f / defence_;

		if (now_strenght_ <= 0) {
			now_strenght_ = 0;
			max_strenght_ = 0;
			defence_ = 0;
		}
	}

	enemy_->TakeDamage(damage);
}

void Player::Fire(int gun) const {
	float radians_angle = enemy_->GetAngle() * kPI / 180.0f;
	sf::Vector2f direction(cos(radians_angle), sin(radians_angle));

	if (gun == -1) {
		if (kick_->IsCanUsed() && guns_[now_gun_]->IsCanUsed()) {
			RayHit hit = Raycast(now_map_, enemy_->GetPosition(), direction, false, enemy_, 1);
			if (hit.cell == 1) now_map_->SetNewOnGrid(hit.map_position.x, hit.map_position.y, kWallLayer, 0);
			kick_->Ussing(nullptr, 0);
		}
	} else {
		if (guns_[now_gun_]->IsCanUsed() && kick_->IsCanUsed()) {
			RayHit hit = Raycast(now_map_, enemy_->GetPosition(), direction, true, enemy_, guns_[now_gun_]->GetMaxDistance(), pitch_);
			float dist = hit.sprite && hit.sprite->GetSpriteType() != SpriteType::NPC ? dist = sqrt(GETDIST(hit.sprite->GetPosition(), enemy_->GetPosition())) : 0;
			guns_[now_gun_]->Ussing(dynamic_cast<Enemy*>(hit.sprite), dist);
		}
	}
}

const Sprite* const Player::Dialog() const {
	float radians_angle = enemy_->GetAngle() * kPI / 180.0f;
	sf::Vector2f direction(cos(radians_angle), sin(radians_angle));

	RayHit hit = Raycast(now_map_, enemy_->GetPosition(), direction, true, enemy_, 1, pitch_);
	return hit.sprite && hit.sprite->GetSpriteType() == SpriteType::NPC ? hit.sprite : nullptr;
}

void Player::SwapGun(bool flag) {
	int delta = flag ? 1 : -1;
	now_gun_ = (now_gun_ + delta + 3) % 3;

	while (!guns_[now_gun_]) {
		now_gun_ = (now_gun_ + delta + 3) % 3;
	}
}

const PlayerDef Player::GetPlayerDef() const {
	std::vector<int> guns_data;

	for (const auto& it : guns_) {
		if (it != nullptr && it->GetName() != L"Кулак") {
			guns_data.push_back(it->GetId());
		}
	}

	return { enemy_->GetMaxHealpoint(),
	enemy_->GetHealpoint(),
	max_energy_,
	now_energy_,
	defence_,
	max_strenght_,
	now_strenght_,
	patrons_,
	money_,
	details_,
	guns_data };
}

void Player::SetNemMap(Map* _map) { now_map_ = _map; }

void Player::ResetPlayer(const PlayerDef player_def) {
	enemy_->SetMaxHealPoint(player_def.max_healpoint);
	enemy_->SetHealpoint(player_def.now_healpoint);
	max_energy_ = player_def.max_energy;
	now_energy_ = player_def.now_energy;
	defence_ = player_def.defence;
	max_strenght_ = player_def.max_strenght;
	now_strenght_ = player_def.now_strenght;
	patrons_ = player_def.patrons_count;
	money_ = player_def.money;
	details_ = player_def.details;
}

const int Player::GetMoney() const { return money_; }

void Player::SetMoney(int new_money) { money_ = new_money; }

const int Player::GetDetails() const { return details_; }

void Player::SetDetails(int new_details) { details_ = new_details; }

Enemy* const Player::GetEnemy() const { return enemy_; }

const float Player::GetMaxEnergy() const { return max_energy_; }

const float Player::GetNowEnergy() const { return now_energy_; }

void Player::SetMaxEnergy(float value) {
	max_energy_ = value;
	now_energy_ = std::min(max_energy_, now_energy_);
}

void Player::SetDefence(float value) { defence_ = value; }

void Player::SetPatrons(int new_count) { patrons_ = new_count; }

const int Player::GetPatronsCount() const { return patrons_; }

const Item* const Player::GetNowHeal() const { return now_heal_; }

void Player::SetNowHeal(Item* const heal) { now_heal_ = heal; }

void Player::SetMouseSpeed(float value) { mouse_speed_ = value; }

const int Player::GetPositionZ() const { return pos_z_; }

const int Player::GetPitch() const { return pitch_; }

const sf::Vector2f& Player::GetShakeDelta() const { return shake_delta_;}

void Player::SetMaxStrenght(float value) { 
	max_strenght_ = value; 
	now_strenght_ = value;
}

const float Player::GetMaxStrenght() const { return max_strenght_; }

const float Player::GetNowStrenght() const { return now_strenght_; }

const Gun* const Player::GetNowGun() const { return guns_[now_gun_]; }

const Gun* const Player::GetKick() const { return kick_; }

void Player::SetKick(Gun* const gun) { kick_ = gun; }

const float Player::GetMoveSpeed() const { return now_speed_; }

void Player::TakeItem(Itemble* const item, int count) {
	inventory_->TakeItem(item, count);
	now_heal_ = inventory_->TakeMaxHeal();
}

void Player::Heal() {
	if (now_heal_) {
		now_heal_->UseItem(this);
		inventory_->UseItem(now_heal_);
		now_heal_ = inventory_->TakeMaxHeal();
	}
}
