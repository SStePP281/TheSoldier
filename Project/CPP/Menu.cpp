#include "Menu.h"

Menu::Menu(sf::RenderWindow* window, UIManager* ui_manager, Player* player) : 
	ui_manager{ui_manager}, window{window}, is_key_pressed{false}, player{player},
	start_menu_state{[=](float delta_time) {UpdateStartMenu();}, [=]() {Draw();}},
	game_menu_state{[=](float delta_time) {UpdateGameMenu();},[=]() {Draw();} },
	setting_state{ [=](float delta_time) {UpdateSetting();}, [=]() {Draw();} },
	reset_state{ [=](float delta_time) {UpdateResetMenu();}, [=]() {Draw();} } {}

void Menu::InitStartMenu()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &start_menu_state);
	window->setMouseCursorVisible(true);
	ui_manager->InitStartMenu();
	SoundManager::PlayerMusic(MusicType::MenuSound);
}

void Menu::InitGameMenu()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &game_menu_state);
	window->setMouseCursorVisible(true);
	ui_manager->InitGameMenu();
}

void Menu::InitSetting()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &setting_state);
	window->setMouseCursorVisible(true);
	ui_manager->InitSetting();
}

void Menu::InitResetMenu()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &reset_state);
	window->setMouseCursorVisible(true);
	ui_manager->InitResetMenu();
}

void Menu::Stop()
{
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", nullptr);
	window->setMouseCursorVisible(false);
	auto& state = GameState::GetInstance();
	SoundManager::PlayerMusic(state.data.isLevelBase ? MusicType::BaseSound : MusicType::LevelSound);
}

void Menu::Draw() { ui_manager->DrawNow(); }

void Menu::UpdateResetMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		is_key_pressed = false;
		return;
	}

	if (is_key_pressed) return;

	is_key_pressed = true;

	int key = ui_manager->CheckButton();

	if (key == -1) return;

	Stop();

	if (key == 0) {}
	else if (key == 1)
	{
		InitStartMenu();
	}
}

void Menu::UpdateStartMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		is_key_pressed = false;
		return;
	}

	if (is_key_pressed) return;

	is_key_pressed = true;

	int key = ui_manager->CheckButton();

	if (key == -1) return;

	if (key == 0) {}
	else if (key == 1)
	{
		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("RESET_GAME", 0);
	}
	else if (key == 2)
	{
		window->close();
	}

	Stop();
}

void Menu::UpdateGameMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		is_key_pressed = false;
		return;
	}

	if (is_key_pressed) return;

	is_key_pressed = true;

	int key = ui_manager->CheckButton();

	if (key == -1) return;

	if (key == 0)
	{
		Stop();
	}
	else if (key == 1)
	{
		ui_manager->DeleteNow();
		InitSetting();
	}
	else if (key == 2)
	{
		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("SAVE", 0);
		ui_manager->DeleteNow();
		InitStartMenu();
	}
}

void Menu::UpdateSetting()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		is_key_pressed = false;
		return;
	}

	if (is_key_pressed) return;

	is_key_pressed = true;

	int key = ui_manager->CheckButton();

	if (key == -1) return;

	if (key == 0) 
	{
		ui_manager->DeleteNow();
		InitGameMenu();
	}
	else if (key == 1)
	{
		SoundManager::UpdateVolume();
		ui_manager->DeleteNow();
		ui_manager->InitSetting();
	}
	else if (key == 2)
	{
		auto& state = GameState::GetInstance();
		player->mouse_speed = state.data.mouseSpeed;
		ui_manager->DeleteNow();
		ui_manager->InitSetting();
	}
}
