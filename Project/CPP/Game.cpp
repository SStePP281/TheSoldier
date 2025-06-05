#include "Game.h"

Game::Game(sf::RenderWindow* window, MapManager* map_manager) :
	window{ window }, map_manager{ map_manager }, is_key_pressed{false},
	screen_midle_position{ (int)(kScreenWight / 2), (int)(kScreenHeight / 2) }
{
	Init();
	SubscribeEvent();
	menu->InitStartMenu();
}

void Game::Init()
{
	item_manager = new ItemManager();
	ui_manager = new UIManager(window);
	dialog_system = new Dialog(window, ui_manager, item_manager);
	sprite_manager = new SpriteManager(map_manager->GetNowMap(), ui_manager, item_manager);
	player = sprite_manager->GetPlayer();
	menu = new Menu(window, ui_manager, player);
	inventory = new Inventory(window, player, ui_manager);
	renderer = new Renderer(window, player);
	InitPlayer();

	auto& data = Data::GetInstance();
	for (auto b : data.GetInvent()) 
	{ 
		player->TakeItem(item_manager->GetItemble(b.first), b.second);
	}

	auto update = [=](float delta_time) {
		GetInput(delta_time);
		sprite_manager->Update(delta_time);
		};

	auto draw = [=]() {
		renderer->Draw3DView(map_manager->GetNowMap(), sprite_manager->GetDeteachSprite());
		ui_manager->DrawPlayerUI(player);
		};

	play_state = RenderState(std::move(update), std::move(draw));
}

void Game::SubscribeEvent()
{
	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("SWAPLOC", [=](const int level_number) {
		auto& state = GameState::GetInstance();
		state.data.changerCoef = Random::IntRandom(2, 5);
		sf::Vector2f pos = map_manager->NextLocation(level_number);
		sprite_manager->ResetMap(map_manager->GetNowMap(), pos);
		});

	event.Subscribe<int>("RESET_GAME", [=](const int NON) {
		ResetGame();
		auto& state = GameState::GetInstance();
		state.data.isFirstGame = false;
		current_state = &play_state;
		});

	event.Subscribe<int>("WIN_GAME", [=](const int NON) {
		ResetGame();
		auto& state = GameState::GetInstance();
		state.data.isFirstGame = true;
		menu->InitStartMenu();
		});

	event.Subscribe<RenderState*>("SWAP_STATE", [=](RenderState* state) {
		ui_manager->DeleteNow();
		current_state = state ? state : &play_state;
		});

	event.Subscribe<int>("PLAYERDEAD", [=](const int NON) {
		sprite_manager->ResetOldPlayer();

		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("SWAPLOC", kBaseNumber);

		auto& state = GameState::GetInstance();
		state.data.levelNumber--;

		menu->InitResetMenu();
		});
}

Game::~Game()
{
	delete dialog_system;
	delete sprite_manager;
	delete item_manager;
	delete ui_manager;
	delete inventory;
	delete menu;
}

void Game::ResetGame()
{
	auto& state = GameState::GetInstance();
	state.data.killFirst = false;
	state.data.killSecond = false;
	state.data.killTherd = false;

	player->guns[1] = nullptr;
	player->guns[2] = nullptr;

	inventory->TakeItem(item_manager->GetGunByIndex(2), 1);
	player->SetGun(item_manager->GetGunByIndex(2), 1);

	auto& quest_manager = QuestManager::GetInstance();
	quest_manager.DeleteAllQuest();
}

void Game::InitPlayer()
{
	player->SetInventory(inventory);
	player->kick = item_manager->GetGunByIndex(0);
	player->SetGun(item_manager->GetGunByIndex(1), 0);
	player->now_heal = inventory->TakeMaxHeal();

	auto& state = GameState::GetInstance();
	player->mouse_speed = state.data.mouseSpeed;

	auto& data = Data::GetInstance();
	PlayerDef player_def = data.GetPlayerData();

	int i = 1;
	for (auto it : player_def.gunsData)
	{
		player->SetGun(item_manager->GetGunById(it), i++);
	}

	dialog_system->SetPlayer(player);
}

void Game::Editor()
{
	sf::Vector2f player_position{};
	sprite_manager->ResetMap(map_manager->GetNowMap(), player_position);
}

void Game::Save()
{
	map_manager->RewriteSprites(sprite_manager->GetDeteachSprite());
	auto& event = EventSystem::GetInstance();
	event.Trigger<int>("SAVE", 0);
}

void Game::GetInput(const sf::Event& event, float delta_time)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Q)
		{
			inventory->UseInvent();
		}

		if (event.key.code == sf::Keyboard::H)
		{
			player->Heal();
		}
#ifdef REDACT_MODE
		if (event.key.code == sf::Keyboard::P)
		{
			auto& event = EventSystem::GetInstance();
			event.Trigger<int>("SWAPLOC", kBaseNumber);
		}
#endif // REDACT_MODE
		if (event.key.code == sf::Keyboard::Escape)
		{
			menu->InitGameMenu();
		}
	}

	if (event.type == sf::Event::MouseWheelScrolled)
	{
		player->SwapGun(event.mouseWheelScroll.delta > 0);
	}
}

void Game::GetInput(float delta_time)
{
	if (!window->hasFocus()) return;

	float radian_angle = player->enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f vertical_params(cos(radian_angle), sin(radian_angle));
	sf::Vector2f horizontal_params(-vertical_params.y, vertical_params.x);
	sf::Vector2f delta_position(0, 0);
	bool left_shift_pressed = false;

	if (GetAsyncKeyState('A')) { delta_position -= horizontal_params; }
	else if (GetAsyncKeyState('D')) { delta_position += horizontal_params; }
	if (GetAsyncKeyState('W')) { delta_position += vertical_params; }
	else if (GetAsyncKeyState('S')) { delta_position -= vertical_params; }
	if (GetAsyncKeyState('R')) { player->ReloadingGun(); }
	if (GetAsyncKeyState(VK_LSHIFT)) { left_shift_pressed = true; }
	if (GetAsyncKeyState(VK_SPACE)) { player->Jump(); }
	if (GetAsyncKeyState('F')) { player->Fire(); }
	if (GetAsyncKeyState('E'))
	{
		if (Sprite* sprite = player->Dialog(); sprite)
		{
			if (auto npc = sprite_manager->GetNpc(sprite->id); npc)
			{
				dialog_system->Start(npc);
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		player->Fire(0);
	}

	sf::Vector2f mouse_position = ((sf::Vector2f)sf::Mouse::getPosition(*window) - (sf::Vector2f)screen_midle_position) / 2.0f;
	
	player->CheckBoost(left_shift_pressed, delta_time);
	player->Move(delta_position, delta_time);
	player->UpdateMouseData( mouse_position, delta_time);
	sf::Mouse::setPosition(screen_midle_position, *window);
}

void Game::MakeCycle(float delta_time)
{
	current_state->Update(delta_time);
	current_state->Draw();
	SoundManager::Update();
}
