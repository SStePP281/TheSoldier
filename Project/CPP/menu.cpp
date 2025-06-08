#include "menu.h"

Menu::Menu(sf::RenderWindow* window, UIManager* ui_manager, Player* player) : 
	ui_manager_{ui_manager}, window_{window}, is_key_pressed_{false}, player_{player},
	start_menu_state_{[=](float delta_time) {UpdateStartMenu();}, [=]() {Draw();}},
	game_menu_state_{[=](float delta_time) {UpdateGameMenu();},[=]() {Draw();} },
	setting_state_{ [=](float delta_time) {UpdateSetting();}, [=]() {Draw();} },
	reset_state_{ [=](float delta_time) {UpdateResetMenu();}, [=]() {Draw();} } {}

void Menu::InitStartMenu() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &start_menu_state_);
	window_->setMouseCursorVisible(true);
	ui_manager_->InitStartMenu();
	SoundManager::PlayerMusic(MusicType::MenuSound);
}

void Menu::InitGameMenu() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &game_menu_state_);
	window_->setMouseCursorVisible(true);
	ui_manager_->InitGameMenu();
}

void Menu::InitSetting() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &setting_state_);
	window_->setMouseCursorVisible(true);
	ui_manager_->InitSetting();
}

void Menu::InitResetMenu() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &reset_state_);
	window_->setMouseCursorVisible(true);
	ui_manager_->InitResetMenu();
}

void Menu::Stop() {
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", nullptr);
	window_->setMouseCursorVisible(false);
	auto& state = GameState::GetInstance();
	SoundManager::PlayerMusic(state.data.is_level_base ? MusicType::BaseSound : MusicType::LevelSound);
}

void Menu::Draw() const { ui_manager_->DrawNow(); }

void Menu::UpdateResetMenu() {
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		is_key_pressed_ = false;
		return;
	}

	if (is_key_pressed_) return;

	is_key_pressed_ = true;

	int key = ui_manager_->CheckButton();

	if (key == -1) return;

	Stop();

	if (key == 0) {}
	else if (key == 1) {
		InitStartMenu();
	}
}

void Menu::UpdateStartMenu() {
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		is_key_pressed_ = false;
		return;
	}

	if (is_key_pressed_) return;

	is_key_pressed_ = true;

	int key = ui_manager_->CheckButton();

	if (key == -1) return;

	if (key == 0) {}
	else if (key == 1) {
		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("RESET_GAME", 0);
	} else if (key == 2) {
		window_->close();
	}

	Stop();
}

void Menu::UpdateGameMenu() {
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		is_key_pressed_ = false;
		return;
	}

	if (is_key_pressed_) return;

	is_key_pressed_ = true;

	int key = ui_manager_->CheckButton();

	if (key == -1) return;

	if (key == 0) {
		Stop();
	} else if (key == 1){
		ui_manager_->DeleteNow();
		InitSetting();
	} else if (key == 2) {
		auto& event = EventSystem::GetInstance();
		event.Trigger<int>("SAVE", 0);
		ui_manager_->DeleteNow();
		InitStartMenu();
	}
}

void Menu::UpdateSetting() {
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		is_key_pressed_ = false;
		return;
	}

	if (is_key_pressed_) return;

	is_key_pressed_ = true;

	int key = ui_manager_->CheckButton();

	if (key == -1) return;

	if (key == 0)  {
		ui_manager_->DeleteNow();
		InitGameMenu();
	} else if (key == 1) {
		SoundManager::UpdateVolume();
		ui_manager_->DeleteNow();
		ui_manager_->InitSetting();
	} else if (key == 2) {
		auto& state = GameState::GetInstance();
		player_->SetMouseSpeed(state.data.mouse_speed);
		ui_manager_->DeleteNow();
		ui_manager_->InitSetting();
	}
}
