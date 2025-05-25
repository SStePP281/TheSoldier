#include "SoundManager.h"
#include "Randomizer.h"
#include "DataBase.h"

SoundManager::SoundManager() {}

std::vector<std::unique_ptr<sf::Sound>> SoundManager::sounds{};

sf::Music SoundManager::music{};

void SoundManager::playSound(sf::SoundBuffer& buffer, bool isLoop)
{
	for (size_t i = 0; i < sounds.size(); i++)
	{
		if (sounds[i]->getBuffer() == &buffer)
		{
			sounds[i]->stop();
			sounds[i]->play();
			return;
		}	
	}

	auto& state = GameState::getInstance();

	sounds.push_back(std::make_unique<sf::Sound>(buffer));
	sounds.back()->setLoop(isLoop);
	sounds.back()->setVolume((float)state.data.effectVolume);
	sounds.back()->play();
}

void SoundManager::playerMusic(MusicType type)
{
	std::string filePath = "Sound/background";

	if (type == MenuSound)
	{
		filePath += "1";
	}
	else if (type == BaseSound)
	{
		filePath += "2";
	}
	else if (type == LevelSound)
	{
		filePath += std::to_string(Random::intRandom(3, 9));
	}
	else if (type == StartIntro)
	{
		filePath = "Sound/startIntroMusic";
	}
	else if (type == EndIntro)
	{
		filePath = "Sound/endIntroMusic";
	}

	auto& state = GameState::getInstance();

	if (!music.openFromFile(filePath + ".ogg")) return;
	music.setLoop(true);
	music.setVolume((float)state.data.soundVolume);
	music.play();
}

void SoundManager::stopAllSound()
{
	for (size_t i = 0; i < sounds.size(); i++)
	{
		sounds[i]->stop();
	}

	sounds.clear();
	music.stop();
}

void SoundManager::update()
{
	size_t i = 0;

	while (i < sounds.size())
	{
		if (sounds[i]->getStatus() == sf::Sound::Stopped)
		{
			sounds.erase(i + sounds.begin());
		}
		else
		{
			i++;
		}
	}
}

void SoundManager::updateVolume()
{
	auto& state = GameState::getInstance();

	music.setVolume((float)state.data.soundVolume);

	for (size_t i = 0; i < sounds.size(); i++)
	{
		sounds[i]->setVolume((float)state.data.effectVolume);
	}
}