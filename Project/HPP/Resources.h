#pragma once
#ifndef RESOURCES
#define RESOURCES

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Audio.hpp>
#include <array>

class Resources
{
private:
	static void loadGun(std::string baseName, int index);
public:
	static sf::Image gameIcon;
	static sf::Image cursorImage;
	static sf::Texture dialogBackground;
	static sf::Texture tradeBackground;
	static sf::Texture menuBackground;
	static sf::Texture inventoryBackground;
	static sf::Texture textures;
	static sf::Texture skyTextures;
	static sf::Image textureImage;
	static sf::Texture spriteIcon;
	static std::vector<sf::Texture> spritesTextures;
	static sf::SoundBuffer takeDamage;
	static sf::SoundBuffer buttonClick;

	static sf::Texture itembleIcon;
	static sf::SoundBuffer gunCantShoutSound;

	static std::vector<std::vector<sf::Texture>> gunsResetAnim;
	static std::vector<std::vector<sf::Texture>> gunsFireAnim;
	static std::vector<sf::Texture> gunsBaseText;
	static std::vector<sf::SoundBuffer> gunsShutSound;
	static std::vector<sf::SoundBuffer> gunsResetSound;

	static sf::Font UIFont;

	static void initResources();
};

#endif // !RESOURCES
