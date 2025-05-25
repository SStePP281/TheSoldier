#include "Menu.h"

Menu::Menu(sf::RenderWindow* _window, UIManager* _uiManager, Player* _player) : 
	uiManager{_uiManager}, window{_window}, isKeyPressed{false}, player{_player},
	startMenuState{[=](float deltaTime) {updateStartMenu();}, [=]() {draw();}},
	gameMenuState{[=](float deltaTime) {updateGameMenu();},[=]() {draw();} },
	settingState{ [=](float deltaTime) {updateSetting();}, [=]() {draw();} },
	resetState{ [=](float deltaTime) {updateResetMenu();}, [=]() {draw();} } {}

void Menu::initStartMenu()
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", &startMenuState);
	window->setMouseCursorVisible(true);
	uiManager->initStartMenu();
	SoundManager::playerMusic(MenuSound);
}

void Menu::initGameMenu()
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", &gameMenuState);
	window->setMouseCursorVisible(true);
	uiManager->initGameMenu();
}

void Menu::initSetting()
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", &settingState);
	window->setMouseCursorVisible(true);
	uiManager->initSetting();
}

void Menu::initResetMenu()
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", &resetState);
	window->setMouseCursorVisible(true);
	uiManager->initResetMenu();
}

void Menu::stop()
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", nullptr);
	window->setMouseCursorVisible(false);
	auto& state = GameState::getInstance();
	SoundManager::playerMusic(state.data.isLevelBase ? BaseSound : LevelSound);
}

void Menu::draw() { uiManager->drawNow(); }

void Menu::updateResetMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isKeyPressed = false;
		return;
	}

	if (isKeyPressed) return;

	isKeyPressed = true;

	int key = uiManager->checkButton();

	if (key == -1) return;

	stop();
	if (key == 0) {}
	else if (key == 1)
	{
		initStartMenu();
	}
}

void Menu::updateStartMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isKeyPressed = false;
		return;
	}

	if (isKeyPressed) return;

	isKeyPressed = true;

	int key = uiManager->checkButton();

	if (key == -1) return;

	if (key == 0) {}
	else if (key == 1)
	{
		auto& event = EventSystem::getInstance();
		event.trigger<int>("RESET_GAME", 0);
	}
	else if (key == 2)
	{
		window->close();
	}

	stop();
}

void Menu::updateGameMenu()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isKeyPressed = false;
		return;
	}

	if (isKeyPressed) return;

	isKeyPressed = true;

	int key = uiManager->checkButton();

	if (key == -1) return;

	if (key == 0)
	{
		stop();
	}
	else if (key == 1)
	{
		uiManager->deleteNow();
		initSetting();
	}
	else if (key == 2)
	{
		auto& event = EventSystem::getInstance();
		event.trigger<int>("SAVE", 0);
		uiManager->deleteNow();
		initStartMenu();
	}
}

void Menu::updateSetting()
{
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isKeyPressed = false;
		return;
	}

	if (isKeyPressed) return;

	isKeyPressed = true;

	int key = uiManager->checkButton();

	if (key == -1) return;

	if (key == 0) 
	{
		uiManager->deleteNow();
		initGameMenu();
	}
	else if (key == 1)
	{
		SoundManager::updateVolume();
		uiManager->deleteNow();
		uiManager->initSetting();
	}
	else if (key == 2)
	{
		auto& state = GameState::getInstance();
		player->mouseSpeed = state.data.mouseSpeed;
		uiManager->deleteNow();
		uiManager->initSetting();
	}
}
