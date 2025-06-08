#include "sound_manager.h"
#include "randomizer.h"
#include "data_base.h"

SoundManager::SoundManager() {}

std::vector<std::unique_ptr<sf::Sound>> SoundManager::sounds_{};

sf::Music SoundManager::music_{};

void SoundManager::PlaySounds(const sf::SoundBuffer& buffer, bool is_loop) {
	for (size_t i = 0; i < sounds_.size(); i++) {
		if (sounds_[i]->getBuffer() == &buffer) {
			sounds_[i]->stop();
			sounds_[i]->play();
			return;
		}	
	}

	auto& state = GameState::GetInstance();

	sounds_.push_back(std::make_unique<sf::Sound>(buffer));
	sounds_.back()->setLoop(is_loop);
	sounds_.back()->setVolume((float)state.data.effect_volume);
	sounds_.back()->play();
}

void SoundManager::PlayerMusic(MusicType type) {
	std::string file_path = "Sound/background";

	if (type == MusicType::MenuSound) {
		file_path += "1";
	} else if (type == MusicType::BaseSound) {
		file_path += "2";
	} else if (type == MusicType::LevelSound) {
		file_path += std::to_string(Random::IntRandom(3, 9));
	} else if (type == MusicType::StartIntro) {
		file_path = "Sound/startIntroMusic";
	} else if (type == MusicType::EndIntro) {
		file_path = "Sound/endIntroMusic";
	}

	auto& state = GameState::GetInstance();

	if (!music_.openFromFile(file_path + ".ogg")) return;
	music_.setLoop(true);
	music_.setVolume((float)state.data.sound_volume);
	music_.play();
}

void SoundManager::StopAllSound() {
	for (size_t i = 0; i < sounds_.size(); i++) {
		sounds_[i]->stop();
	}

	sounds_.clear();
	music_.stop();
}

void SoundManager::Update() {
	size_t i = 0;

	while (i < sounds_.size()) {
		if (sounds_[i]->getStatus() == sf::Sound::Stopped) {
			sounds_.erase(i + sounds_.begin());
		} else {
			i++;
		}
	}
}

void SoundManager::UpdateVolume() {
	auto& state = GameState::GetInstance();

	music_.setVolume((float)state.data.sound_volume);

	for (size_t i = 0; i < sounds_.size(); i++) {
		sounds_[i]->setVolume((float)state.data.effect_volume);
	}
}
