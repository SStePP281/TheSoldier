#include "DialogSystem.h"

Dialog::Dialog(sf::RenderWindow* window, UIManager* ui_manager,
	ItemManager* weapon_manager) : window{ window },
	ui_manager{ ui_manager }, weapon_manager{ weapon_manager },
	npc{ nullptr }, player{ nullptr }, 
	dialog_state{ [=](float deltaTime) { Update(); }, [=]() { Draw();}} {}

void Dialog::SetPlayer(Player* player) { player = player; }

void Dialog::Start(Npc* _npc)
{
	if (!_npc) return;
	npc = _npc;

	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", &dialog_state);
	window->setMouseCursorVisible(true);

	npc->SetEndFunc([=]() {Stop();});
	npc->Init();
}

void Dialog::Stop()
{
	window->setMouseCursorVisible(false);
	npc = nullptr;
	auto& event = EventSystem::GetInstance();
	event.Trigger<RenderState*>("SWAP_STATE", nullptr);
}

void Dialog::Update()
{
	if (!window->hasFocus()) return;
	
	static bool is_mouse_down = false;
	bool is_press = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	if (is_press && !is_mouse_down)
	{
		int key = ui_manager->CheckButton();

		if (key != -1)
		{
			npc->Update(key);
		}

		is_mouse_down = true;
	}
	if (!is_press)
	{
		is_mouse_down = false;
	}
}

void Dialog::Draw()
{
	ui_manager->DrawNow();
}