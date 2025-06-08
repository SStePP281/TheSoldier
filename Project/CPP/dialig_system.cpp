#include "dialog_system.h"

Dialog::Dialog(sf::RenderWindow* window, UIManager* ui_manager, ItemManager* weapon_manager) : 
    window_{ window }, ui_manager_{ ui_manager }, weapon_manager_{ weapon_manager },
    npc_{ nullptr }, player_{ nullptr },
    dialog_state_{ [=](float deltaTime) { Update(); }, [=]() { Draw();} } {
}

void Dialog::SetPlayer(const Player* player) {
    player = player;
}

void Dialog::Start(Npc* _npc) {
    if (!_npc) {
        return;
    }
    npc_ = _npc;

    auto& event = EventSystem::GetInstance();
    event.Trigger<RenderState*>("SWAP_STATE", &dialog_state_);
    window_->setMouseCursorVisible(true);

    npc_->SetEndFunc([=]() { Stop(); });
    npc_->Init();
}

void Dialog::Stop() {
    window_->setMouseCursorVisible(false);
    npc_ = nullptr;
    auto& event = EventSystem::GetInstance();
    event.Trigger<RenderState*>("SWAP_STATE", nullptr);
}

void Dialog::Update() const {
    if (!window_->hasFocus()) {
        return;
    }

    static bool is_mouse_down = false;
    bool is_press = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (is_press && !is_mouse_down) {
        int key = ui_manager_->CheckButton();

        if (key != -1) {
            npc_->Update(key);
        }

        is_mouse_down = true;
    }

    if (!is_press) {
        is_mouse_down = false;
    }
}

void Dialog::Draw() const {
    ui_manager_->DrawNow();
}