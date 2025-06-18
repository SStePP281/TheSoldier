#include "sprite.h"
#include "ui_manager.h"
#include "item_manager.h"
#include "map.h"
#include "weapon.h"
#include "player.h"

Sprite::Sprite(const SpriteDef& sprite_def, const MapSprite& map_sprite, int id) :
	sprite_def_{ sprite_def }, map_sprite_{ map_sprite }, id_{ id }, texture_size_{ 0 }, texture_{ nullptr } {
	if (sprite_def.type == SpriteType::Decoration) {
		texture_ = &Resources::sprites_textures[sprite_def.texture];
		texture_size_ = texture_->getSize().y;
	} else if (sprite_def.texture != -1) {
		texture_ = &Resources::sprites_textures[sprite_def.texture];
		texture_size_ = texture_->getSize().x / 8;
	}
}

const std::pair<int, bool> Sprite::GetTextIndex() const { return { 0, false }; }

void Sprite::SetPosition(const sf::Vector2f& new_position) { map_sprite_.position = new_position; }

const sf::Vector2f& Sprite::GetPosition() const { return map_sprite_.position; }

const float Sprite::GetTextureSize() const { return texture_size_; }

void Sprite::SetAngle(float new_angle) { map_sprite_.angle = new_angle; }

const float Sprite::GetAngle() const { return map_sprite_.angle; }

const float Sprite::GetSize() const { return sprite_def_.size; }

const int Sprite::GetId() const { return id_; }

void Enemy::SetHealpoint(float now_healpoint) { 
	map_sprite_.now_heal_point = now_healpoint;
	map_sprite_.now_heal_point = std::min(enemy_def_.max_healpoint, map_sprite_.now_heal_point);
}

const float Enemy::GetHealpoint() const { return map_sprite_.now_heal_point; }

void Enemy::SetMaxHealPoint(float new_healpoint) {
	enemy_def_.max_healpoint = new_healpoint;
	map_sprite_.now_heal_point = std::min(enemy_def_.max_healpoint, map_sprite_.now_heal_point);
}

const float Enemy::GetMaxHealpoint() const { return enemy_def_.max_healpoint; }

const float Enemy::GetMoveSpeed() const { return enemy_def_.speed; }

const float Enemy::GetAttackDistance() const { return enemy_def_.attack_dist; }

const int Enemy::GetMidleDrop() const { return enemy_def_.midle_drop; }

void Enemy::SetIsCanAttcak(bool value) { is_can_attack_ = value; }

const bool Enemy::GetIsCanAttack() const { return is_can_attack_; }

const bool Enemy::GetIsAttack() const { return is_attack_; }

void Enemy::SetIsAttcak(bool value) { is_attack_ = value; }

const SpriteType& Sprite::GetSpriteType() const { return sprite_def_.type; }

const MapSprite& Sprite::GetMapSprite() const { return map_sprite_; }

const sf::Texture* const Sprite::GetTexture() const { return texture_; }

const int Sprite::GetTextureId() const { return sprite_def_.texture; }

void Sprite::SetTextureSize(int new_size) { texture_size_ = new_size; }

const bool Sprite::IsDirectional() const { return sprite_def_.is_directional; }

void Sprite::SetBlockmapCoords(std::set<std::tuple<int, int>> coords) { blockmap_coords_ = coords; }

const std::set<std::tuple<int, int>>& Sprite::GetBlockmapCoords() const { return blockmap_coords_; }

Enemy::Enemy(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, int id) :
	Sprite(sprite_def, map_sprite, id), enemy_def_{ enemy_def }, state_{ EnemyState::Stay }, time_atecked_{ 0.5f }, 
	is_damaged_{ false }, now_time_attack_{ 0 }, is_can_attack_{false}, is_attack_{false} {
	float frame_time = 1.0f / enemy_def.speed;
	auto stay = Animation<int>({ {0,0} });

	int index = 1;
	Animation<int> run;
	if (enemy_def.is_can_run) {
		run = Animation<int>({
		{ frame_time * 0, index++ },
		{ frame_time * 1, index++ },
		{ frame_time * 2, index++ },
		{ frame_time * 3, index },
		{ frame_time * 4, index++ } });
	} else {
		run = stay;
	}

	frame_time = 1.0f / 3;

	auto atack = Animation<int>({
	{ frame_time * 0, index++ },
	{ frame_time * 1, index++ },
	{ frame_time * 2, index },
	{ frame_time * 3, index++ } });

	auto dead = Animation<int>({ {0, index} });

	animr_ = Animator<int>(0, { stay, run, atack, dead });
}

const std::pair<int, bool> Enemy::GetTextIndex() const { return { animr_.Get(), is_damaged_ }; }

void Enemy::Death() {
	animr_.SetAnimation(3);
	is_damaged_ = false;
}

void Enemy::Attack(Player* const player) {
	player->TakeDamage(enemy_def_.damage);
}

void Enemy::Move(Map* const map, const sf::Vector2f& move) {
	if (move == sf::Vector2f()) return;

	float x_offset = move.x > 0 ? sprite_def_.size / 2.0f : -sprite_def_.size / 2.0f;
	float y_offset = move.y > 0 ? sprite_def_.size / 2.0f : -sprite_def_.size / 2.0f;
	sf::Vector2f delta_x = { map_sprite_.position.x + move.x + x_offset, map_sprite_.position.y };
	sf::Vector2f delta_y = { map_sprite_.position.x, map_sprite_.position.y + move.y + y_offset };

	if (!CheckCollision(map, delta_x, true)) {
		map_sprite_.position.x += move.x;
	}
	if (!CheckCollision(map, delta_y, false)) {
		map_sprite_.position.y += move.y;
	}

	map->SetupBlockmap(this);
}

void Enemy::Update(float delta_time) {
	if (state_ == EnemyState::Dead) return;

	animr_.Update(delta_time);

	UpdateTimeSinceLastAttack(delta_time);
	UpdateTimeSinceDamaged(delta_time);
}

void Enemy::UpdateTimeSinceLastAttack(float delta_time) {
	if (now_time_attack_ >= enemy_def_.time_bettwen_attack) {
		is_can_attack_ = true;
	} else {
		now_time_attack_ += delta_time;
	}
}

void Enemy::UpdateTimeSinceDamaged(float delta_time) {
	if (time_atecked_ < 0.5f) {
		time_atecked_ += delta_time;
	} else {
		is_damaged_ = false;
	}
}

const bool Enemy::CanChangeState() const {
	return animr_.Get() == 0 || animr_.is_lopping == true;
}

void Enemy::ChangeState(EnemyState new_state) {
	if (state_ == EnemyState::Dead) return;

	if (new_state == EnemyState::Stay) {
		animr_.SetAnimation(0);
	} else if (new_state == EnemyState::Run) {
		if (state_ != EnemyState::Run) {
			animr_.SetAnimation(1, true);
		}
	} else if (new_state == EnemyState::Attack) {
		is_attack_ = true;
		is_can_attack_ = false;
		animr_.SetAnimation(2);
		now_time_attack_ = 0.0f;
	} else if (new_state == EnemyState::Dead) {
		Death();
	}

	state_ = new_state;
}

void Enemy::TakeDamage(float damage) {
	if (state_ == EnemyState::Dead) return;

	map_sprite_.now_heal_point -= damage;
	SoundManager::PlaySounds(Resources::take_damage);

	is_damaged_ = true;
	time_atecked_ = 0;
}

const EnemyState Enemy::DetermineNewState(float dist) const {
	if (dist < enemy_def_.attack_dist) {
		return EnemyState::Attack;
	} else if (dist < kTrigerDist) {
		return EnemyState::Run;
	} else {
		return EnemyState::Stay;
	}
}

void Enemy::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time) {
	if (is_attack_) { return; }
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite_.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run) {
		if (Random::BitRandom() > 0.7f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def_.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	} else if (new_state == EnemyState::Attack) {
		if (!CanChangeState()) return;

		if (is_can_attack_) {
			ChangeState(new_state);
		} else {
			if (Random::BitRandom() > 0.3f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	} else if (new_state == EnemyState::Stay) {
		ChangeState(new_state);
	}
}

const bool Enemy::CheckCollision(Map* const map, const sf::Vector2f& new_position, bool x_axis) const {
	sf::Vector2f this_size{ sprite_def_.size / 2.0f, sprite_def_.size / 2.0f };
	sf::Vector2f this_start = new_position - this_size;
	sf::Vector2f this_end = new_position + this_size;

	if (map->GetOnGrid((int)new_position.x, (int)new_position.y, kWallLayer)) { return true; }

	for (const auto& sp : map->GetBlockMap((sf::Vector2i)new_position)) {
		if (sp->GetSize() == 0.f || sp == this) continue;

		sf::Vector2f sprite_size = { sp->GetSize() / 2.f, sp->GetSize() / 2.f };
		sf::Vector2f sprite_start = sp->GetPosition() - sprite_size;
		sf::Vector2f sprite_end = sp->GetPosition() + sprite_size;

		if (x_axis) {
			float px = new_position.x;
			float py0 = this_start.y, py1 = this_end.y, magnitude = py1 - py0;
			float t1 = (sprite_start.y - py0) / magnitude;
			float t2 = (sprite_end.y - py0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool y_intersect = exit > entry && exit > 0.f && entry < 1.f;
			if (px >= sprite_start.x && px <= sprite_end.x && y_intersect) { return true; }
		} else {
			float py = new_position.y;
			float px0 = this_start.x, px1 = this_end.x, magnitude = px1 - px0;
			float t1 = (sprite_start.x - px0) / magnitude;
			float t2 = (sprite_end.x - px0) / magnitude;

			float entry = std::fmin(t1, t2);
			float exit = std::fmax(t1, t2);

			bool xIntersect = exit > entry && exit > 0.f && entry < 1.f;
			if (py >= sprite_start.y && py <= sprite_end.y && xIntersect) { return true; }
		}
	}

	return false;
}

Converter::Converter(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& _converter_def, int id) :
	Enemy(sprite_def, map_sprite, enemy_def, id), converter_def_{ _converter_def } {
	now_spawn_count_ = (int)(converter_def_.max_spawn_count * map_sprite.now_heal_point / enemy_def.max_healpoint);
	texture_size_ = texture_->getSize().y / 2;
	Animation<int> stay({ {0.0f, 0} });
	Animation<int> attack({ {0.0f, 0}, {enemy_def.time_bettwen_attack, 0 }, {enemy_def.time_bettwen_attack, 0 } });
	Animation<int> death({ {0.0f, 1} });
	animr_ = Animator<int>(0, { stay, {}, attack, death });
}

void Converter::TakeDamage(float damege) {
	if (now_spawn_count_ != 0) return;

	Enemy::TakeDamage(damege);
}

void Converter::Death() {
	auto& state = GameState::GetInstance();
	if (sprite_def_.texture == 13) {
		state.data.kill_first_converter = true;
	} else if (sprite_def_.texture == 14) {
		state.data.kill_second_converter = true;
	} else if (sprite_def_.texture == 15) {
		state.data.kill_therd_converter = true;
	}

	is_damaged_ = false;
	animr_.SetAnimation(3);
	auto& event = EventSystem::GetInstance();
	event.Trigger<sf::Vector2f>("SPAWN_PORTAL", map_sprite_.position);
}

void Converter::Attack(Player* const plaer) {
	if (now_spawn_count_ == 0) { return; }
	now_spawn_count_--;

	auto& event = EventSystem::GetInstance();
	event.Trigger<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", { converter_def_.calling_index[Random::IntRandom(0, converter_def_.calling_index.size() - 1)], (sf::Vector2i)map_sprite_.position });
}

void Converter::ChangeState(EnemyState new_state) {
	if (state_ == EnemyState::Dead) return;

	if (new_state == EnemyState::Spawn) {
		is_attack_ = true;
		is_can_attack_ = false;
		animr_.SetAnimation(2);
		now_time_attack_ = 0.0f;
	} else if (new_state == EnemyState::Dead) {
		Death();
	}

	state_ = new_state;
}

void Converter::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time) {
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite_.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run && !is_attack_) {
		if (Random::BitRandom() > 0.7f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def_.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	} else if (new_state == EnemyState::Spawn) {
		if (!CanChangeState()) return;

		if (is_can_attack_) {
			ChangeState(new_state);
		} else {
			if (Random::BitRandom() > 0.3f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	}
	else if (new_state == EnemyState::Stay) {
		ChangeState(new_state);
	}
}

const EnemyState Converter::DetermineNewState(float dist) const {
	if (dist < kSpawnRadius) {
		return EnemyState::Spawn;
	} else if (dist < kTrigerDist) {
		return EnemyState::Run;
	} else {
		return EnemyState::Stay;
	}
}

const bool Converter::CanChangeState() const {
	return !is_attack_ || now_spawn_count_ > 0;
}

Boss::Boss(const SpriteDef& sprite_def, const MapSprite& map_sprite, const EnemyDef& enemy_def, const ConverterDef& _converter_def, int id) :
	Enemy(sprite_def, map_sprite, enemy_def, id), converter_def_{ _converter_def } {
	now_spawn_count_ = (int)(converter_def_.max_spawn_count * map_sprite.now_heal_point / enemy_def.max_healpoint);
}

void Boss::Death() {
	is_damaged_ = false;
	animr_.SetAnimation(3);
	auto& event = EventSystem::GetInstance();
	event.Trigger<sf::Vector2f>("WIN_GAME", map_sprite_.position);
}

void Boss::Attack(Player* const player) {
	if (state_ == EnemyState::Attack) {
		Enemy::Attack(player);
	} else if (state_ == EnemyState::Spawn) {
		if (now_spawn_count_ == 0) return;

		now_spawn_count_--;

		auto& event = EventSystem::GetInstance();
		event.Trigger<std::pair<int, sf::Vector2i>>("SPAWN_ENEMY", { converter_def_.calling_index[Random::IntRandom(0, converter_def_.calling_index.size() - 1)], (sf::Vector2i)map_sprite_.position });
	}
}

void Boss::EnemyMechenic(float dist, const sf::Vector2f& to_player_dir, Map* const now_map, float delta_time) {
	if (is_attack_) { return; }
	auto new_state = DetermineNewState(dist);

	float angle = map_sprite_.angle * kPI / 180.0f;
	sf::Vector2f direction{ cos(angle), sin(angle) };

	if (new_state == EnemyState::Run) {
		if (Random::BitRandom() > 0.7f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		sf::Vector2f delta_move = enemy_def_.speed * delta_time * direction;
		Move(now_map, delta_move);

		ChangeState(new_state);
	} else if (new_state == EnemyState::Spawn || new_state == EnemyState::Attack) {
		if (!CanChangeState()) return;

		if (is_can_attack_) {
			ChangeState(new_state);
		} else {
			if (Random::BitRandom() > 0.3f) map_sprite_.angle = std::atan2(to_player_dir.y, to_player_dir.x) * 180.0f / kPI;
		}
	} else if (new_state == EnemyState::Stay) {
		ChangeState(new_state);
	}
}

void Boss::ChangeState(EnemyState new_state) {
	if (state_ == EnemyState::Dead) return;

	if (new_state == EnemyState::Stay) {
		animr_.SetAnimation(0);
	} else if (new_state == EnemyState::Spawn) {
		animr_.SetAnimation(0);
		is_attack_ = true;
		is_can_attack_ = false;
		now_time_attack_ = 0.0f;
	} else if (new_state == EnemyState::Attack) {
		is_attack_ = true;
		is_can_attack_ = false;
		animr_.SetAnimation(2);
		now_time_attack_ = 0.0f;
	} else if (new_state == EnemyState::Run) {
		if (state_ != EnemyState::Run) {
			animr_.SetAnimation(1, true);
		}
	}
	else if (new_state == EnemyState::Dead) {
		Death();
	}

	state_ = new_state;
}

const EnemyState Boss::DetermineNewState(float dist) const {
	int hp_percent = (int)(abs(map_sprite_.now_heal_point - 1) / enemy_def_.max_healpoint * 100.0f);
	int a = hp_percent / 25;
	int b = a % 2;
	if (b == 1) {
		if (dist < enemy_def_.attack_dist) {
			return EnemyState::Attack;
		} else if (dist < kTrigerDist) {
			return EnemyState::Run;
		} else {
			return EnemyState::Stay;
		}
	} else {
		if (dist < kSpawnRadius) {
			return EnemyState::Spawn;
		} else {
			return EnemyState::Stay;
		}
	}
}

Npc::Npc(const SpriteDef& sprite_def, const MapSprite& map_sprite, UIManager* const ui_manager, Player* const player, const NpcDef& npc_def, int id) :
	Sprite(sprite_def, map_sprite, id), npc_def_{ npc_def }, player_{ player }, ui_manager_{ ui_manager }, now_key_{ 1 } {
	texture_size_ = texture_->getSize().y;
}

void Npc::SetEndFunc(std::function<void()>&& _endFunc) { end_func_ = _endFunc; }

void Npc::Init() {
	auto& data = Data::GetInstance();
	auto keys = data.GetKeys(npc_def_.id_key, now_key_);

	std::map<int, std::wstring, std::greater<int>> variants;

	for (int i = 0; i < keys.size(); i++) {
		auto d = data.GetText(npc_def_.id_key, keys[i]);
		variants[d.second] = d.first;
	}

	ui_manager_->InitDialog(variants, sprite_def_.name);
}

void Npc::Stop() {
	now_key_ = 1;
	end_func_();
	end_func_ = nullptr;
	ui_manager_->DeleteNow();
}

void Npc::Use() {}

void Npc::Update(int choose_key)  {
	now_key_ = choose_key;
	Check();
}

void Npc::Check() {
	if (now_key_ == 0) {
		Stop();
	}
	else {
		ui_manager_->DeleteNow();
		Init();
	}
}

FuncNpc::FuncNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def, ItemManager* const item_manager,
	UIManager* const ui_manager, Player* const _player, int _id) :
	Npc(sprite_def, map_sprite, ui_manager, _player, npc_def, _id), is_func_{ false }, choose_{ -1 }, item_manager_{ item_manager } {}

void FuncNpc::Stop() {
	Npc::Stop();

	is_func_ = false;
	choose_ = -1;
}

void FuncNpc::Update(int choose_key) {
	now_key_ = choose_key;

	if (now_key_ == 999) {
		is_func_ = true;
		Init();
		return;
	}

	if (!is_func_) {
		Npc::Check();
	} else {
		Check();
	}
}

void FuncNpc::Check() {
	if (now_key_ == -100) {
		Stop();
	} else if (now_key_ == -200) {
		Use();
	} else {
		choose_ = now_key_;
	}
}

TradeNpc::TradeNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const TraderDef& _tradeDef, const NpcDef& npc_def,
	ItemManager* const item_manager, UIManager* const ui_manager, Player* const _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id), trade_def_{ _tradeDef } {}

void TradeNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		ui_manager_->DeleteNow();

		auto result = std::map<int, Itemble*>();
		for (int i = 0; i < trade_def_.title.size(); i++) {
			result[trade_def_.title[i]] = item_manager_->GetItemble(trade_def_.title[i]);
		}

		ui_manager_->InitTrade(result, player_);
	}
}

void TradeNpc::Use() {
	if (choose_ == -1) return;

	Itemble* itemble = item_manager_->GetItemble(choose_);

	if (itemble->GetCost() > player_->GetMoney()) return;

	player_->SetMoney(player_->GetMoney() - itemble->GetCost());
	player_->TakeItem(itemble);

	choose_ = -1;

	Init();
}

TravelerNpc::TravelerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* const ui_manager, ItemManager* const item_manager, Player* const _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id) {}

void TravelerNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		ui_manager_->DeleteNow();

		auto result = std::map<int, Itemble*>();
		result[0] = item_manager_->GetItemble(traveler_defs[0].id);

		int max_size = 1;
		auto& state = GameState::GetInstance();
		int level_number = state.data.level_number;
		if (state.data.kill_first_converter &&
			state.data.kill_second_converter &&
			state.data.kill_therd_converter) {
			max_size = 5;
		} else {
			max_size = std::min(level_number / 3, 4);
		}

		for (int i = 1; i < max_size; i++) {
			result[i] = item_manager_->GetItemble(traveler_defs[i].id);
		}

		ui_manager_->InitTrade(result, player_);
	}
}

void TravelerNpc::Use() {
	if (choose_ == -1) return;

	if (traveler_defs[choose_].cost > player_->GetMoney()) return;

	player_->SetMoney(player_->GetMoney() - traveler_defs[choose_].cost);

	int temp = traveler_defs[choose_].effect;
	Stop();

	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SWAPLOC", temp);
}

ChangerNpc::ChangerNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* const ui_manager, ItemManager* const item_manager, Player* const _player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, _player, _id) {
	auto& state = GameState::GetInstance();
	coef_ = state.data.changer_coef;
}

void ChangerNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		ui_manager_->DeleteNow();

		ui_manager_->InitChanger(coef_, player_);
	}
}

void ChangerNpc::Use() {
	if (player_->GetDetails() - 10 < 0) return;

	player_->SetDetails(player_->GetDetails() - 10);
	player_->SetMoney(player_->GetMoney() + 10 * coef_);
	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.UpdateQuests(QuestType::CollectionMoney, 10 * coef_);

	Init();
}

PortalNpc::PortalNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* const ui_manager, ItemManager* const item_manager, Player* const player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id) {}

void PortalNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		FuncNpc::Stop();

		Use();
	}
}

void PortalNpc::Use() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SWAPLOC", kBaseNumber);
}

MechanicNpc::MechanicNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* const ui_manager, ItemManager* const item_manager, Player* const player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id), type_upgade_{ -1 } {}

void MechanicNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		ui_manager_->DeleteNow();

		ui_manager_->InitMechanic(player_, (choose_ == -1 ? nullptr : player_->GetGun(choose_)));
	}
}

void MechanicNpc::Stop() {
	FuncNpc::Stop();
	type_upgade_ = -1;
}

void MechanicNpc::Use() {
	if (choose_ == -1 || type_upgade_ == -1) return;

	Gun* now_gun = player_->GetGun(choose_);

	if (player_->GetMoney() < 50 || player_->GetDetails() < 15 || now_gun->GetUpgradeCount() >= 5) return;

	player_->SetMoney(player_->GetMoney() - 50);
	player_->SetDetails(player_->GetDetails() - 15);

	Improve* improve = nullptr;
	if (type_upgade_ == 101) {
		improve = now_gun->DeleteImprove(ImproveType::Damage);
		now_gun->SetDamage(now_gun->GetDamage() + 3);
	} else if (type_upgade_ == 102) {
		improve = now_gun->DeleteImprove(ImproveType::Magazin);
		now_gun->SetMaxPatronsCount(now_gun->GetMaxPatronsCount() + 5);
	} else if (type_upgade_ == 103) {
		improve = now_gun->DeleteImprove(ImproveType::Spread);
		now_gun->SetMaxRadius(now_gun->GetMaxRadius() - 3);
	}

	now_gun->TrySetImprove(improve);
	now_gun->TakeOneUpgrade();

	type_upgade_ = -1;
	choose_ = -1;
	Init();
}

void MechanicNpc::Check() {
	if (now_key_ == -100) {
		Stop();
	} else if (now_key_ == -200) {
		Use();
	} else if (now_key_ == 1 || now_key_ == 2) {
		choose_ = now_key_;
		Init();
		return;
	} else {
		type_upgade_ = now_key_;
	}
}

QuestNpc::QuestNpc(const SpriteDef& sprite_def, const MapSprite& map_sprite, const NpcDef& npc_def,
	UIManager* const ui_manager, ItemManager* const item_manager, Player* const player, int _id) :
	FuncNpc(sprite_def, map_sprite, npc_def, item_manager, ui_manager, player, _id) {}

void QuestNpc::Init() {
	if (!is_func_) {
		Npc::Init();
	} else {
		ui_manager_->DeleteNow();

		auto& quest_manager = QuestManager::GetInstance();
		ui_manager_->InitQuest((choose_ == -1 ? nullptr : quest_manager.GetQuests()[choose_ - 1]), player_);
	}
}

void QuestNpc::Use() {
	if (choose_ == -1) return;

	auto& quest_manager = QuestManager::GetInstance();
	player_->SetMoney(player_->GetMoney() + quest_manager.DeleteQuest(quest_manager.GetQuests()[choose_ - 1]));

	choose_ = -1;
	Init();
}

void QuestNpc::Check() {
	if (now_key_ == -100) {
		Stop();
	} else if (now_key_ == -200) {
		Use();
	} else if (now_key_ == -300) {
		int type = Random::IntRandom(0, 2);
		QuestData quest_data;
		quest_data.progress = 0;
		quest_data.type = QuestType(type);

		if (type == 0) {
			quest_data.target = Random::IntRandom(5, 15);
			quest_data.reward_coins = Random::IntRandom(quest_data.target * 10, quest_data.target * 15);
		} else if (type == 1) {
			quest_data.target = Random::IntRandom(100, 300);
			quest_data.reward_coins = Random::IntRandom((int)(quest_data.target * 1.1f), (int)(quest_data.target * 1.3f));
		} else if (type == 2) {
			quest_data.target = Random::IntRandom(10, 25);
			quest_data.reward_coins = quest_data.reward_coins = Random::IntRandom(quest_data.target * 10, quest_data.target * 15);
		}

		auto& quest_manager = QuestManager::GetInstance();
		quest_manager.AddQuest(quest_data);

		Init();
	} else if (now_key_ == 1 || now_key_ == 2 || now_key_ == 3) {
		choose_ = now_key_;
		Init();
	}
}
