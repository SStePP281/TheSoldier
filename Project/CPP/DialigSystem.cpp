#include "DialogSystem.h"

Dialog::Dialog(sf::RenderWindow* _window, UIManager* _uiManager,
	ItemManager* _weaponManager) : window{ _window },
	uiManager{ _uiManager }, weaponManager{ _weaponManager },
	npc{ nullptr }, player{ nullptr }, 
	dialogState{ [=](float deltaTime) { update(); }, [=]() { draw();}} {}

void Dialog::setPlayer(Player* _player) { player = _player; }

void Dialog::start(Npc* _npc)
{
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", &dialogState);
	window->setMouseCursorVisible(true);
	npc = _npc;
	npc->setEndFunc([=]() {stop();});
	npc->init();
}

void Dialog::stop()
{
	window->setMouseCursorVisible(false);
	npc = nullptr;
	auto& event = EventSystem::getInstance();
	event.trigger<RenderState*>("SWAP_STATE", nullptr);
}

void Dialog::update()
{
	if (!window->hasFocus()) return;
	
	static bool isMouseDown = false;
	bool isPress = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	if (isPress && !isMouseDown)
	{
		int key = uiManager->checkButton();

		if (key != -1)
		{
			npc->update(key);
		}

		isMouseDown = true;
	}
	if (!isPress)
	{
		isMouseDown = false;
	}
}

void Dialog::draw()
{
	uiManager->drawNow();
}