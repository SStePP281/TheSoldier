#include "sfml_extantion.h"
#include "randomizer.h"
#include "data_base.h"

Group::Group(sf::RectangleShape shape, sf::Text text) : shape{ shape }, text{ text } {
	Centrlized();
}

void Group::SetString(std::wstring data) {
	text.setString(data);
	Centrlized();
}

void Group::SetPosition(sf::Vector2f position) {
	shape.setPosition(position);
	text.setPosition({ position.x, position.y - text.getCharacterSize() / 4 });
}

void Group::SetSize(sf::Vector2f size) {
	shape.setSize(size);
	Centrlized();
}

void Group::Move(sf::Vector2f position) {
	SetPosition({ GetPosition().x + position.x, GetPosition().y + position.y }); 
}

sf::Vector2f Group::GetSize() { return shape.getSize(); }

sf::Vector2f Group::GetPosition() { return shape.getPosition(); }

void Group::Centrlized() {
	shape.setOrigin({ shape.getLocalBounds().width / 2, shape.getLocalBounds().height / 2 });
	text.setOrigin({ text.getLocalBounds().width / 2, text.getLocalBounds().height / 2 });
	text.setPosition({ shape.getPosition().x, shape.getPosition().y - text.getCharacterSize() / 4 });
}

Button::Button(sf::RectangleShape& shape, sf::Text& text) {
	group.shape = shape;
	group.text = text;
}

Button::Button(Group& group) : group{ group } {}

sf::Vector2f Button::GetPosition() { return group.GetPosition(); }

sf::Vector2f Button::GetSize() { return group.GetSize(); }

void Button::SetString(std::wstring data) { group.SetString(data); }

void Button::Move(sf::Vector2f delta) {
	group.shape.move(delta);
	group.text.move(delta);
}

void Button::SetFillColor(sf::Color color) {
	group.shape.setFillColor(color);
}

void Button::SetPosition(sf::Vector2f position) { group.SetPosition(position); }

void Button::SetSize(sf::Vector2f size) {
	group.SetSize(size);
	group.Centrlized();
}

bool Button::IsClicked(sf::Vector2i& mouse_position) {
	if (!func_) return false;

	sf::Vector2f pos = group.GetPosition();
	sf::Vector2f size = { group.GetSize().x * group.shape.getScale().x, group.GetSize().y * group.shape.getScale().y };
	float x0 = pos.x - size.x / 2, x1 = pos.x + size.x / 2;
	float y0 = pos.y - size.y / 2, y1 = pos.y + size.y / 2;

	if (mouse_position.x >= x0 && mouse_position.x <= x1 && mouse_position.y >= y0 && mouse_position.y <= y1) {
		return true;
	}

	return false;
}

void Button::SetFunc(std::function<void()>&& _fn) { func_ = _fn; }

void Button::SetTexture(sf::Texture* texture) {
	group.shape.setTexture(texture);
}

void Button::SetTextureRect(sf::IntRect rect) {
	group.shape.setTextureRect(rect);
}

void Button::Use() {
	if (func_) {
		func_();
		SoundManager::PlaySounds(Resources::button_click);
	}
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(group.shape, states);
	target.draw(group.text, states);
}
