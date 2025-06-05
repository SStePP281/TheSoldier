
#ifndef SOUNDM
#define SOUNDM

#include "CONST.h"
#include <memory>
#include <SFML/System/Vector3.hpp>
#include <SFML/Audio.hpp>

class SoundManager
{
public:
	SoundManager();
	static void PlaySounds(const sf::SoundBuffer& buffer, bool is_loop = false);
	static void PlayerMusic(MusicType type);
	static void StopAllSound();
	static void Update();
	static void UpdateVolume();
private:
	static std::vector<std::unique_ptr<sf::Sound>> sounds;
	static sf::Music music;
};
#endif // !SOUNDM
