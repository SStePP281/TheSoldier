#include "editor.h"
#include "sprite_manager.h"
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>

Editor::Editor(sf::RenderWindow* window, sf::RenderWindow* editor_window, MapManager* const map_manager) :
	map_manager_{ map_manager }, window_{window}, editor_window_{editor_window}, now_value_{1}, now_layer_{kWallLayer},
	window_view_{window->getView()}, editor_view_{editor_window->getView()} {
	CreateTextureButton();
	CreateSpriteButton();
}

void Editor::CreateTextureButton() {
	sf::RectangleShape shape(sf::Vector2f{ kTextureSize, kTextureSize });
	shape.setScale({ 0.5f ,0.5f });
	Group base_grop(shape, {});
	Button b(base_grop);
	b.SetTexture(&Resources::textures);

	int x = 0, y = 0;
	for (; x < (int)Resources::textures.getSize().x / kTextureSize * 5; x++, y++) {
		b.SetPosition({ (x % kCountRowTexture) * (float)kIconSize + kIconSize / 2, 
						(y / kCountRowTexture) * (float)kIconSize });
		b.SetTextureRect({ {(int)(x % kTextureCount * kTextureSize), 
							(int)(x / kTextureCount * kTextureSize)},
							{kTextureSize, kTextureSize} });

		b.SetFunc([=]() { now_value_ = x + 1; });

		buttons_.push_back(std::make_shared<Button>(b));
	}
}

void Editor::CreateSpriteButton()  {
	int  y = (int)buttons_.size() / kCountRowTexture * kCountRowTexture + kCountRowTexture;

	sf::RectangleShape shape(sf::Vector2f{ kIconSize, kIconSize });
	Group base_group(shape, {});
	Button b(base_group);
	b.SetTexture(&Resources::sprite_icon);

	for (int x = 0; x < sprite_defs.size() - 1; x++, y++) {
		b.SetPosition({ (float)kIconSize * (x % kCountRowTexture) + kIconSize / 2,
						y / kCountRowTexture * (float)kIconSize });
		b.SetTextureRect({ { kIconSize * x, 0}, {kIconSize, kIconSize} });

		b.SetFunc([=]() { now_sprite_def_ = sprite_defs[x + 1]; });
		buttons_.push_back(std::make_shared<Button>(b));
	}
}

void Editor::TakeEditInput(sf::Event& event) {
	if (!editor_window_->hasFocus()) return;

	editor_mouse_position_ = sf::Mouse::getPosition(*editor_window_);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		EditorWindowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled) {
		float delta_scrol = -5 * event.mouseWheelScroll.delta;
		editor_view_.move({ 0, delta_scrol });
	}

	editor_window_->setView(editor_view_);
}

void Editor::TakeWindowInput(sf::Event& event) {
	if (!window_->hasFocus()) return;

	window_mouse_position_ = sf::Mouse::getPosition(*window_);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		WindowStateRightClick();
	} else {
		WindowStateNoRightClick();	
	}


	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		WindowStateLeftClick();
	}

	if (event.type == sf::Event::MouseWheelScrolled) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
			ScrollAndCntr(0.1f * event.mouseWheelScroll.delta);
		} else {
			window_view_.zoom(1.0f - 0.1f * event.mouseWheelScroll.delta);
		}
	}

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Tab) {
			now_layer_++;
			now_layer_ = now_layer_ % kAllLayerCount;
		}
	}

	window_->setView(window_view_);
}

void Editor::WindowStateRightClick() {
	if (is_first_mouse_) {
		last_mouse_pos_ = window_mouse_position_;
		is_first_mouse_ = false;
	} else {
		sf::Vector2i delta_mouse = window_mouse_position_ - last_mouse_pos_;

		window_view_.setCenter(window_view_.getCenter() - (sf::Vector2f)delta_mouse);

		sf::Mouse::setPosition(last_mouse_pos_, *window_);
	}

	window_->setMouseCursorVisible(false);
}

void Editor::WindowStateNoRightClick() {
	is_first_mouse_ = true;
	window_->setMouseCursorVisible(true);
}

void Editor::ScrollAndCntr(float delta) const {
	if (now_layer_ != kSpeiteLayer) return;

	sf::Vector2f world_position = window_->mapPixelToCoords(window_mouse_position_);
	sf::Vector2i mapPosition = GetMapPos(world_position);

	if (map_manager_->GetNowMap()->IsCellEmpty(mapPosition)) return;

	map_manager_->GetNowMap()->RotateSprite(mapPosition, delta * 10);
}

void Editor::WindowStateLeftClick() const {
	sf::Vector2f world_position = window_->mapPixelToCoords(window_mouse_position_);
	sf::Vector2i map_position = GetMapPos(world_position);

	if (now_layer_ != kSpeiteLayer) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
			map_manager_->GetNowMap()->SetNewOnGrid(map_position.x, map_position.y, now_layer_, 0);
		} else {
			map_manager_->GetNowMap()->SetNewOnGrid(map_position.x, map_position.y, now_layer_, now_value_);
		}
	} else {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
			map_manager_->GetNowMap()->DeleteMapSprite(map_position);
		} else {
			if (now_sprite_def_.texture != -1) {
				if (map_manager_->GetNowMap()->IsCellEmpty(map_position)) {
					float now_healpoint = 10;

					if (now_sprite_def_.type != SpriteType::NPC && now_sprite_def_.type != SpriteType::Decoration) {
						now_healpoint = enemy_defs[now_sprite_def_.texture + 1].max_healpoint;
					}

					MapSprite map_sprite = { now_sprite_def_.texture + 1, { map_position.x + 0.5f, map_position.y + 0.5f }, -90.0f, now_healpoint };
					map_manager_->GetNowMap()->SetMapSprite(map_sprite);
				}
			}
		}
	}
}

void Editor::EditorWindowStateLeftClick() const {
	sf::Vector2i world_positon = (sf::Vector2i)editor_window_->mapPixelToCoords(editor_mouse_position_);

	for (int i = 0; i < buttons_.size(); i++) {
		if (buttons_[i]->IsClicked(world_positon)) {
			buttons_[i]->Use();
			break;
		}
	}
}

void Editor::DrawEditor() const {
	for (const auto& b : buttons_) {
		editor_window_->draw(*b.get());
	}
}

const sf::Vector2i& Editor::GetMapPos(const sf::Vector2f& world_position) const {
	return sf::Vector2i((int)floor(world_position.x - 0.1f) / kTextureSize,
						(int)floor(world_position.y - 0.1f) / kTextureSize);
}

const int Editor::DrawerLayer() const { return now_layer_; }