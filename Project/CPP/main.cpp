#include "editor.h"
#include "resources.h"
#include "game.h"
#include "map_manager.h"
#include <sfeMovie/Movie.hpp>
#include <SFML/Window/Cursor.hpp>

enum class State{Editor, Game};

int main() {
	Resources::initResources();

	sf::RenderWindow window(sf::VideoMode(kScreenWight, kScreenHeight), "Game"/*, sf::Style::Fullscreen*/);
	window.setIcon(Resources::game_icon.getSize().x, Resources::game_icon.getSize().y, Resources::game_icon.getPixelsPtr());
	window.setFramerateLimit(60);
	sf::Cursor cur{};
	cur.loadFromPixels(Resources::cursor_image.getPixelsPtr(), Resources::cursor_image.getSize(), sf::Vector2u(0, 0));
	window.setMouseCursor(cur);

#ifdef REDACT_MODE
	sf::RenderWindow editor_window(sf::VideoMode(450,500), "Editor");
	editor_window.setPosition(sf::Vector2i(0, 0));
	editor_window.setActive(false);
	editor_window.setVisible(false);
#endif // REDACT_MODE

	auto& event = EventSystem::GetInstance();
	event.Subscribe<int>("RESET_GAME", [&](const int NON) {
		sfe::Movie movie;

		if (!movie.openFromFile("Sound/startIntroVideo.mp4")) return;
		movie.fit(0.0f, 0.0f, (float)window.getSize().x, (float)window.getSize().y);
		movie.play();

		SoundManager::StopAllSound();
		SoundManager::PlayerMusic(MusicType::StartIntro);

		while (movie.getStatus() == sfe::Status::Playing) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
					return;
				}
			}

			window.clear();	
			movie.update();
			window.draw(movie);
			window.display();
		}

		window.clear();
		});

	event.Subscribe<int>("WIN_GAME", [&](const int NON) {
		sfe::Movie movie;
		if (!movie.openFromFile("Sound/endIntroVideo.mp4")) return;
		movie.fit(0.0f, 0.0f, (float)window.getSize().x, (float)window.getSize().y);
		movie.play();
		SoundManager::StopAllSound();
		SoundManager::PlayerMusic(MusicType::EndIntro);

		while (movie.getStatus() == sfe::Status::Playing) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
					return;
				}
			}

			window.clear();
			movie.update();
			window.draw(movie);
			window.display();
		}

		window.clear();
		});

	State state = State::Game;
	std::unique_ptr<MapManager> map_manager = std::make_unique<MapManager>(&window);
	map_manager->Load();

#ifdef REDACT_MODE
	std::unique_ptr<Editor> editor = std::make_unique<Editor>(&window, &editor_window, map_manager.get());
#endif // REDACT_MODE

	std::unique_ptr<Game> game = std::make_unique<Game>(&window, map_manager.get());

	sf::Clock gameClock;
	float deltaTime = 0;

	while (window.isOpen()) {
#ifdef REDACT_MODE
		sf::Event event;
		while (window.pollEvent(event)) {
			if (state == State::Editor) {
				editor->TakeWindowInput(event);
			}
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::RControl) {
					if (state == State::Editor) {
						state = State::Game;
						window.setMouseCursorVisible(false);
						window.setView(window.getDefaultView());
						game->Editor();
						editor_window.setActive(false);
						editor_window.setVisible(false);
					} else {
						state = State::Editor;
						window.setMouseCursorVisible(true);
						editor_window.setVisible(true);
						editor_window.setActive(true);
					}
				}
			}

			if (state == State::Game) {
				game->GetInput(event, deltaTime);
			}
		}

		if (state == State::Editor) {
			while (editor_window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					editor_window.close();
				}

				editor->TakeEditInput(event);
			}
		}

		window.clear();

		if (state == State::Game) {
			game->MakeCycle(deltaTime);
		} else {	
			editor_window.clear();

			map_manager->DrawMap(editor->DrawerLayer());
			editor->DrawEditor();
			
			editor_window.display();
		}
#endif // REDACT_MODE
#ifndef REDACT_MODE
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			game->GetInput(event, deltaTime);
		}

		window.clear();
		game->MakeCycle(deltaTime);
#endif // !REDACT_MODE
		window.display();
		window.setTitle("SOLDIER " + std::to_string(1.0f / deltaTime));
		deltaTime = gameClock.getElapsedTime().asSeconds();
		gameClock.restart();
	}

	game->Save();

	return 0;
}