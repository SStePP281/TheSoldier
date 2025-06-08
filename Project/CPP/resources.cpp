#include "resources.h"

sf::Image Resources::game_icon{};
sf::Image Resources::cursor_image{};
sf::Image Resources::texture_image{};
sf::Texture Resources::textures{};
sf::Texture Resources::dialog_background{};
sf::Texture Resources::trade_background{};
sf::Texture Resources::menu_background{};
sf::Texture Resources::inventory_background{};
sf::Texture Resources::sprite_icon{};
sf::Texture Resources::itemble_icon{};
sf::Texture Resources::sky_textures{};
std::vector<sf::Texture> Resources::sprites_textures{};
std::vector<sf::Texture> Resources::guns_base_text = std::vector<sf::Texture>(8);
std::vector<std::vector<sf::Texture>> Resources::guns_reset_anim = std::vector<std::vector<sf::Texture>>(8);
std::vector<std::vector<sf::Texture>> Resources::guns_fire_anim = std::vector<std::vector<sf::Texture>>(8);
sf::SoundBuffer Resources::button_click{};
sf::SoundBuffer Resources::take_damage{};
sf::SoundBuffer Resources::gun_cant_shout_sound{};
std::vector<sf::SoundBuffer> Resources::guns_shut_sound = std::vector<sf::SoundBuffer>(8);
std::vector<sf::SoundBuffer> Resources::guns_reset_sound = std::vector<sf::SoundBuffer>(8);
sf::Font Resources::ui_font{};

void loadFor(std::string base_name, std::string format, std::vector<sf::Texture>* data) {
	int i = 1;
	while (true) {
		std::string try_name = base_name + std::to_string(i) + format;
		sf::Texture text;
		if (!text.loadFromFile(try_name)) break;
		i++;
		data->push_back(text);
	}
}

void Resources::LoadGun(std::string base_name, int index) {
	std::string shut_name = "Texture/" + base_name + "FireTexture";
	loadFor(shut_name, ".png", &guns_fire_anim[index]);

	std::string reset_name = "Texture/" + base_name + "ResetTexture";
	loadFor(reset_name, ".png", &guns_reset_anim[index]);

	std::string base_text_name = "Texture/" + base_name + "BaseTexture.png";
	guns_base_text[index].loadFromFile(base_text_name);

	std::string reset_sound_name = "Sound/" + base_name + "ResetSound.ogg";
	guns_reset_sound[index].loadFromFile(reset_sound_name);

	std::string shut_sound_name = "Sound/" + base_name + "ShutSound.ogg";
	guns_shut_sound[index].loadFromFile(shut_sound_name);
}

void Resources::initResources() {
	if (!texture_image.loadFromFile("Texture/wall_texture.png")) throw "TextureLoadError!";
	if (!game_icon.loadFromFile("Texture/gameIcon.png")) throw "TextureLoadError!";
	if (!cursor_image.loadFromFile("Texture/cursorTexture.png")) throw "TextureLoadError!";
	if (!dialog_background.loadFromFile("Texture/dialogBackground.png")) throw "TextureLoadError!";
	if (!trade_background.loadFromFile("Texture/tradeBackground.png")) throw "TextureLoadError!";
	if (!menu_background.loadFromFile("Texture/menuBackground.png")) throw "TextureLoadError!";
	if (!inventory_background.loadFromFile("Texture/inventoryBackground.png")) throw "TextureLoadError!";
	if (!textures.loadFromFile("Texture/wall_texture.png")) throw "TextureLoadError!";
	if (!sky_textures.loadFromFile("Texture/sky_texture.png")) throw "TextureLoadError!";
	if (!sprite_icon.loadFromFile("Texture/enemysIcon.png")) throw "TextureLoadError!";
	loadFor("Texture/enemy", ".png", & sprites_textures);
	if (!take_damage.loadFromFile("Sound/takeDamage.ogg")) throw "TextureLoadError!";
	if (!button_click.loadFromFile("Sound/buttonClick.ogg")) throw "TextureLoadError!";
	sky_textures.setRepeated(true);
	if (!itemble_icon.loadFromFile("Texture/itemIcon.png")) throw "TextureLoadError";
	if (!gun_cant_shout_sound.loadFromFile("Sound/gunCantShutSound.ogg")) throw "TextureLoadError";
	for (int i = 0; i < 8; i++) { LoadGun("gun" + std::to_string(i), i); }
	if (!ui_font.loadFromFile("Texture/dehinted-HandveticaNeue-Regular.ttf")) throw "TextureLoadError!";
}
