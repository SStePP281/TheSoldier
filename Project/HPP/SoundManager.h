#pragma once
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
	static void playSound(sf::SoundBuffer& buffer, bool isLoop = false);
	static void playerMusic(MusicType type);
	static void stopAllSound();
	static void update();
	static void updateVolume();
private:
	static std::vector<std::unique_ptr<sf::Sound>> sounds;
	static sf::Music music;
};
#endif // !SOUNDM