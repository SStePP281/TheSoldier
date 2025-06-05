#include "SoundManager.h"
#include "Randomizer.h"
#include "DataBase.h"

SoundManager::SoundManager() {}

std::vector<std::unique_ptr<sf::Sound>> SoundManager::sounds{};

sf::Music SoundManager::music{};

void SoundManager::PlaySounds(const sf::SoundBuffer& buffer, bool is_loop)
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

	auto& state = GameState::GetInstance();

	sounds.push_back(std::make_unique<sf::Sound>(buffer));
	sounds.back()->setLoop(is_loop);
	sounds.back()->setVolume((float)state.data.effectVolume);
	sounds.back()->play();
}

void SoundManager::PlayerMusic(MusicType type)
{
	std::string file_path = "Sound/background";

	if (type == MusicType::MenuSound)
	{
		file_path += "1";
	}
	else if (type == MusicType::BaseSound)
	{
		file_path += "2";
	}
	else if (type == MusicType::LevelSound)
	{
		file_path += std::to_string(Random::IntRandom(3, 9));
	}
	else if (type == MusicType::StartIntro)
	{
		file_path = "Sound/startIntroMusic";
	}
	else if (type == MusicType::EndIntro)
	{
		file_path = "Sound/endIntroMusic";
	}

	auto& state = GameState::GetInstance();

	if (!music.openFromFile(file_path + ".ogg")) return;
	music.setLoop(true);
	music.setVolume((float)state.data.soundVolume);
	music.play();
}

void SoundManager::StopAllSound()
{
	for (size_t i = 0; i < sounds.size(); i++)
	{
		sounds[i]->stop();
	}

	sounds.clear();
	music.stop();
}

void SoundManager::Update()
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

void SoundManager::UpdateVolume()
{
	auto& state = GameState::GetInstance();

	music.setVolume((float)state.data.soundVolume);

	for (size_t i = 0; i < sounds.size(); i++)
	{
		sounds[i]->setVolume((float)state.data.effectVolume);
	}
}