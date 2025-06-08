#include "game.h"

Game::Game(sf::RenderWindow* window, MapManager* map_manager) :
	window_{ window }, map_manager_{ map_manager }, is_key_pressed_{false},
	screen_midle_position_{ (int)(kScreenWight / 2), (int)(kScreenHeight / 2) } {
	Init();
	SubscribeEvent();
	menu_->InitStartMenu();
}

void Game::Init() {
	item_manager_ = new ItemManager();
	ui_manager_ = new UIManager(window_);
	dialog_system_ = new Dialog(window_, ui_manager_, item_manager_);
	sprite_manager_ = new SpriteManager(map_manager_->GetNowMap(), ui_manager_, item_manager_);
	player_ = sprite_manager_->GetPlayer();
	menu_ = new Menu(window_, ui_manager_, player_);
	inventory_ = new Inventory(window_, player_, ui_manager_);
	renderer_ = new Renderer(window_, player_);
	InitPlayer();

	for (const auto& b : Data::GetInstance().GetInvent()) {
		player_->TakeItem(item_manager_->GetItemble(b.first), b.second);
	}

	auto update = [=](float delta_time) {
		GetInput(delta_time);
		sprite_manager_->Update(delta_time);
		};

	auto draw = [=]() {
		renderer_->Draw3DView(map_manager_->GetNowMap(), sprite_manager_->GetDeteachSprite());
		ui_manager_->DrawPlayerUI(player_);
		};

	play_state_ = RenderState(std::move(update), std::move(draw));
}

void Game::SubscribeEvent() {
	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SWAPLOC", [=](const int level_number) {
		auto& state = GameState::GetInstance();
		state.data.changer_coef = Random::IntRandom(2, 5);
		sf::Vector2f pos = map_manager_->NextLocation(level_number);
		sprite_manager_->ResetMap(map_manager_->GetNowMap(), pos);
		});

	event.Subscribe<int>("RESET_GAME", [=](const int NON) {
		ResetGame();
		auto& state = GameState::GetInstance();
		state.data.is_first_game = false;
		current_state_ = &play_state_;
		});

	event.Subscribe<int>("WIN_GAME", [=](const int NON) {
		ResetGame();
		auto& state = GameState::GetInstance();
		state.data.is_first_game = true;
		menu_->InitStartMenu();
		});

	event.Subscribe<RenderState*>("SWAP_STATE", [=](RenderState* state) {
		ui_manager_->DeleteNow();
		current_state_ = state ? state : &play_state_;
		});

	event.Subscribe<int>("PLAYERDEAD", [=](const int NON) {
		sprite_manager_->ResetOldPlayer();

		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("SWAPLOC", kBaseNumber);

		auto& state = GameState::GetInstance();
		state.data.level_number--;

		menu_->InitResetMenu();
		});
}

Game::~Game() {
	delete dialog_system_;
	delete sprite_manager_;
	delete item_manager_;
	delete ui_manager_;
	delete inventory_;
	delete menu_;
}

void Game::ResetGame() const {
	auto& state = GameState::GetInstance();
	state.data.kill_first_converter = false;
	state.data.kill_second_converter = false;
	state.data.kill_therd_converter = false;

	player_->SetGun(nullptr, 1);
	player_->SetGun(nullptr, 2);

	inventory_->TakeItem(item_manager_->GetGunByIndex(2), 1);
	player_->SetGun(item_manager_->GetGunByIndex(2), 1);

	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.DeleteAllQuest();
}

void Game::InitPlayer() const {
	player_->SetInventory(inventory_);
	player_->SetKick(item_manager_->GetGunByIndex(0));
	player_->SetGun(item_manager_->GetGunByIndex(1), 0);
	player_->SetNowHeal(inventory_->TakeMaxHeal());

	auto& state = GameState::GetInstance();
	player_->SetMouseSpeed(state.data.mouse_speed);

	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	int i = 1;
	for (const auto& it : player_def.guns_data) {
		player_->SetGun(item_manager_->GetGunById(it), i++);
	}

	dialog_system_->SetPlayer(player_);
}

void Game::Editor() const {
	sf::Vector2f player_position{};
	sprite_manager_->ResetMap(map_manager_->GetNowMap(), player_position);
}

void Game::Save() const {
	map_manager_->RewriteSprites(sprite_manager_->GetDeteachSprite());
	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SAVE", 0);
}

void Game::GetInput(const sf::Event& event, float delta_time) const {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Q) {
			inventory_->UseInvent();
		}

		if (event.key.code == sf::Keyboard::H) {
			player_->Heal();
		}
#ifdef REDACT_MODE
		if (event.key.code == sf::Keyboard::P) {
			auto& event = EventSystem::GetInstance();
			event.Trigger<int>("SWAPLOC", kBaseNumber);
		}
#endif // REDACT_MODE
		if (event.key.code == sf::Keyboard::Escape) {
			menu_->InitGameMenu();
		}
	}

	if (event.type == sf::Event::MouseWheelScrolled) {
		player_->SwapGun(event.mouseWheelScroll.delta > 0);
	}
}

void Game::GetInput(float delta_time) const
{
	if (!window_->hasFocus()) return;

	float radian_angle = player_->GetEnemy()->GetAngle() * kPI / 180.0f;
	sf::Vector2f vertical_params(cos(radian_angle), sin(radian_angle));
	sf::Vector2f horizontal_params(-vertical_params.y, vertical_params.x);
	sf::Vector2f delta_position(0, 0);
	bool left_shift_pressed = false;

	if (GetAsyncKeyState('A')) { delta_position -= horizontal_params; }
	else if (GetAsyncKeyState('D')) { delta_position += horizontal_params; }
	if (GetAsyncKeyState('W')) { delta_position += vertical_params; }
	else if (GetAsyncKeyState('S')) { delta_position -= vertical_params; }
	if (GetAsyncKeyState('R')) { player_->ReloadingGun(); }
	if (GetAsyncKeyState(VK_LSHIFT)) { left_shift_pressed = true; }
	if (GetAsyncKeyState(VK_SPACE)) { player_->Jump(); }
	if (GetAsyncKeyState('F')) { player_->Fire(); }
	if (GetAsyncKeyState('E')) {
		if (const Sprite* const sprite = player_->Dialog(); sprite) {
			if (auto npc = sprite_manager_->GetNpc(sprite->GetId()); npc) {
				dialog_system_->Start(npc);
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		player_->Fire(0);
	}

	sf::Vector2f mouse_position = ((sf::Vector2f)sf::Mouse::getPosition(*window_) - (sf::Vector2f)screen_midle_position_) / 2.0f;
	
	player_->CheckBoost(left_shift_pressed, delta_time);
	player_->Move(delta_position, delta_time);
	player_->UpdateMouseData( mouse_position, delta_time);
	sf::Mouse::setPosition(screen_midle_position_, *window_);
}

void Game::MakeCycle(float delta_time) const {
	current_state_->Update(delta_time);
	current_state_->Draw();
	SoundManager::Update();
}
