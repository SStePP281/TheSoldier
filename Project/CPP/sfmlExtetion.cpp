#include "sfmlExtantion.h"

Button::Button(sf::RectangleShape _shape, sf::Text& _text)
{
	group.shape = _shape;
	group.text = _text;
}

Button::Button(Group _group) : group{ _group } {}

sf::Vector2f Button::getPosition() { return group.getPosition(); }

sf::Vector2f Button::getSize() { return group.getSize(); }

void Button::setString(std::wstring data) { group.setString(data); }

void Button::move(sf::Vector2f delta)
{
	group.shape.move(delta);
	group.text.move(delta);
}

void Button::setFillColor(sf::Color color)
{
	group.shape.setFillColor(color);
}

void Button::setPosition(sf::Vector2f pos) { group.setPosition(pos); }

void Button::setSize(sf::Vector2f size)
{
	group.setSize(size);
	group.centrlized();
}

bool Button::isClicked(sf::Vector2i& mousePos)
{
	if (!fn) return false;

	sf::Vector2f pos = group.getPosition();
	sf::Vector2f size = { group.getSize().x * group.shape.getScale().x, group.getSize().y * group.shape.getScale().y };
	float x0 = pos.x - size.x / 2, x1 = pos.x + size.x / 2;
	float y0 = pos.y - size.y / 2, y1 = pos.y + size.y / 2;

	if (mousePos.x >= x0 && mousePos.x <= x1 && mousePos.y >= y0 && mousePos.y <= y1)
	{
		return true;
	}
	return false;
}

void Button::setFunc(std::function<void()>&& _fn) { fn = _fn; }

void Button::setTexture(sf::Texture* text)
{
	group.shape.setTexture(text);
}

void Button::setTextureRect(sf::IntRect rect)
{
	group.shape.setTextureRect(rect);
}

void Button::use()
{
	if (fn)
	{
		fn();
		SoundManager::playSound(Resources::buttonClick);
	}
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(group.shape, states);
	target.draw(group.text, states);
}