#pragma once
#ifndef SFMLEXT
#define SFMLEXT

#include "Resources.h"
#include "SoundManager.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Windows.h>
#include <functional>

struct Vector2iCompare
{
	bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const
	{
		return pow(a.x, 2) + pow(a.y, 2) < pow(b.x, 2) + pow(b.y, 2);
	}
};

struct Group
{
public:
	Group(sf::RectangleShape _shape, sf::Text _text) : shape{ _shape }, text{ _text }
	{
		centrlized();
	}

	Group() = default;

	void setString(std::wstring data)
	{
		text.setString(data);
		centrlized();
	}

	void setPosition(sf::Vector2f position)
	{
		shape.setPosition(position);
		text.setPosition({ position.x, position.y - text.getCharacterSize() / 4 });
	}

	void setSize(sf::Vector2f size)
	{
		shape.setSize(size);
		centrlized();
	}

	void move(sf::Vector2f pos) { setPosition({ getPosition().x + pos.x, getPosition().y + pos.y }); }

	sf::Vector2f getSize() { return shape.getSize(); }
	sf::Vector2f getPosition() { return shape.getPosition(); }

	void centrlized()
	{
		shape.setOrigin({ shape.getLocalBounds().width / 2, shape.getLocalBounds().height / 2 });
		text.setOrigin({ text.getLocalBounds().width / 2, text.getLocalBounds().height / 2 });
		text.setPosition({ shape.getPosition().x, shape.getPosition().y - text.getCharacterSize() / 4 });
	}

	sf::RectangleShape shape;
	sf::Text text;
};

class Button : public sf::Drawable
{
public:
	Button(sf::RectangleShape _shape, sf::Text& _text);
	Button(Group _group);
	Button() = default;

	void move(sf::Vector2f delta);
	void setFillColor(sf::Color color);
	void setPosition(sf::Vector2f);
	void setSize(sf::Vector2f size);
	sf::Vector2f getPosition();
	sf::Vector2f getSize();
	void setString(std::wstring data);
	void setTexture(sf::Texture* text);
	void setTextureRect(sf::IntRect rect);

	void setFunc(std::function<void()>&& _fn);
	bool isClicked(sf::Vector2i& mousePos);
	void use();

	Group group;
private:
	std::function<void()> fn;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // !SFMLEXT