#ifndef RESOURCES
#define RESOURCES

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Audio.hpp>
#include <array>

class Resources {
public:
	static void initResources();

	static sf::Image game_icon;
	static sf::Image cursor_image;
	static sf::Image texture_image;
	static sf::Texture dialog_background;
	static sf::Texture trade_background;
	static sf::Texture menu_background;
	static sf::Texture inventory_background;
	static sf::Texture textures;
	static sf::Texture sky_textures;
	static sf::Texture sprite_icon;
	static sf::Texture itemble_icon;
	static std::vector<sf::Texture> sprites_textures;
	static std::vector<sf::Texture> guns_base_text;
	static std::vector<std::vector<sf::Texture>> guns_reset_anim;
	static std::vector<std::vector<sf::Texture>> guns_fire_anim;
	static sf::SoundBuffer take_damage;
	static sf::SoundBuffer button_click;
	static sf::SoundBuffer gun_cant_shout_sound;
	static std::vector<sf::SoundBuffer> guns_shut_sound;
	static std::vector<sf::SoundBuffer> guns_reset_sound;
	static sf::Font ui_font;
private:
	static void LoadGun(std::string base_name, int index);
};

#endif // !RESOURCES
