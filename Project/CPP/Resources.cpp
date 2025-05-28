#include "Resources.h"
#include <thread>

sf::Image Resources::gameIcon{};
sf::Image Resources::cursorImage{};
sf::Texture Resources::textures{};
sf::Texture Resources::dialogBackground{};
sf::Texture Resources::tradeBackground{};
sf::Texture Resources::menuBackground{};
sf::Texture Resources::inventoryBackground{};
sf::Image Resources::textureImage{};
sf::Texture Resources::spriteIcon{};
std::vector<sf::Texture> Resources::spritesTextures{};
sf::SoundBuffer Resources::buttonClick{};
sf::SoundBuffer Resources::takeDamage{};
sf::Texture Resources::skyTextures{};

sf::Texture Resources::itembleIcon{};
sf::SoundBuffer Resources::gunCantShoutSound{};
std::vector<std::vector<sf::Texture>> Resources::gunsResetAnim = std::vector<std::vector<sf::Texture>>(8);
std::vector<std::vector<sf::Texture>> Resources::gunsFireAnim = std::vector<std::vector<sf::Texture>>(8);
std::vector<sf::Texture> Resources::gunsBaseText = std::vector<sf::Texture>(8);
std::vector<sf::SoundBuffer> Resources::gunsShutSound = std::vector<sf::SoundBuffer>(8);
std::vector<sf::SoundBuffer> Resources::gunsResetSound = std::vector<sf::SoundBuffer>(8);

sf::Font Resources::UIFont{};

void loadFor(std::string baseName, std::string format, std::vector<sf::Texture>* data)
{
	int i = 1;
	while (true)
	{
		std::string tryName = baseName + std::to_string(i) + format;
		sf::Texture text;
		if (!text.loadFromFile(tryName)) break;
		i++;
		data->push_back(text);
	}
}

void Resources::loadGun(std::string baseName, int index)
{
	std::string shutName = "Texture/" + baseName + "FireTexture";
	loadFor(shutName, ".png", &gunsFireAnim[index]);
	std::string resetName = "Texture/" + baseName + "ResetTexture";
	loadFor(resetName, ".png", &gunsResetAnim[index]);
	std::string baseTextName = "Texture/" + baseName + "BaseTexture.png";
	gunsBaseText[index].loadFromFile(baseTextName);
	std::string resetSoundName = "Sound/" + baseName + "ResetSound.ogg";
	gunsResetSound[index].loadFromFile(resetSoundName);
	std::string shutSoundName = "Sound/" + baseName + "ShutSound.ogg";
	gunsShutSound[index].loadFromFile(shutSoundName);
}

void Resources::initResources()
{
	if (!textureImage.loadFromFile("Texture/wall_texture.png")) throw "TextureLoadError!";
	if (!gameIcon.loadFromFile("Texture/gameIcon.png")) throw "TextureLoadError!";
	if (!cursorImage.loadFromFile("Texture/cursorTexture.png")) throw "TextureLoadError!";
	if (!dialogBackground.loadFromFile("Texture/dialogBackground.png")) throw "TextureLoadError!";
	if (!tradeBackground.loadFromFile("Texture/tradeBackground.png")) throw "TextureLoadError!";
	if (!menuBackground.loadFromFile("Texture/menuBackground.png")) throw "TextureLoadError!";
	if (!inventoryBackground.loadFromFile("Texture/inventoryBackground.png")) throw "TextureLoadError!";
	if (!textures.loadFromFile("Texture/wall_texture.png")) throw "TextureLoadError!";
	if (!skyTextures.loadFromFile("Texture/sky_texture.png")) throw "TextureLoadError!";
	if (!spriteIcon.loadFromFile("Texture/enemysIcon.png")) throw "TextureLoadError!";
	loadFor("Texture/enemy", ".png", & spritesTextures);
	if (!takeDamage.loadFromFile("Sound/takeDamage.ogg")) throw "TextureLoadError!";
	if (!buttonClick.loadFromFile("Sound/buttonClick.ogg")) throw "TextureLoadError!";
	skyTextures.setRepeated(true);

	if (!itembleIcon.loadFromFile("Texture/itemIcon.png")) throw "TextureLoadError";
	if (!gunCantShoutSound.loadFromFile("Sound/gunCantShutSound.ogg")) throw "TextureLoadError";

	for (int i = 0; i < 8; i++) { loadGun("gun" + std::to_string(i), i); }

	if (!UIFont.loadFromFile("Texture/dehinted-HandveticaNeue-Regular.ttf")) throw "TextureLoadError!";
}