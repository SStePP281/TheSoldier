#ifndef SFMLEXT
#define SFMLEXT

#include "resources.h"
#include "sound_manager.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Windows.h>
#include <functional>

struct Vector2iCompare {
	bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
		return pow(a.x, 2) + pow(a.y, 2) < pow(b.x, 2) + pow(b.y, 2);
	}
};

struct Group {
public:
	Group(sf::RectangleShape shape, sf::Text text);
	Group() = default;
	void SetString(std::wstring data);
	void SetPosition(sf::Vector2f position);
	void SetSize(sf::Vector2f size);
	void Move(sf::Vector2f pos);
	sf::Vector2f GetSize();
	sf::Vector2f GetPosition();
	void Centrlized();

	sf::RectangleShape shape;
	sf::Text text;
};

class Button : public sf::Drawable {
public:
	Button(sf::RectangleShape& shape, sf::Text& text);
	Button(Group& group);
	Button() = default;
	void Move(sf::Vector2f delta);
	void SetFillColor(sf::Color color);
	void SetPosition(sf::Vector2f);
	void SetSize(sf::Vector2f size);
	sf::Vector2f GetPosition();
	sf::Vector2f GetSize();
	void SetString(std::wstring data);
	void SetTexture(sf::Texture* text);
	void SetTextureRect(sf::IntRect rect);
	void SetFunc(std::function<void()>&& _fn);
	bool IsClicked(sf::Vector2i& mouse_position);
	void Use();

	Group group;
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::function<void()> func_;
};

#endif // !SFMLEXT
