#pragma once
#ifndef RENDERER
#define RENDERER

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <thread>
#include <queue>
#include "Player.h"
#include "Map.h"
#include "Sprite.h"
#include "Raycast.h"
#include "Resources.h"

constexpr int MAX_DETH = 64;
constexpr float PLAYER_FOV = 60.0f, CAMERA_Z = SCREEN_H / 2.0f, ASPECT = (float)SCREEN_W / SCREEN_H * 0.5f, BRIGHTNESTDIST = MAX_DETH / 7.0f;

class ThreadPool
{
public:
	ThreadPool(int threadCount);
	~ThreadPool();
	void addTask(std::function<void()>&& task);
	void waitAll();
	int getThreadCount();
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop = false;
	size_t activeTasks = 0;
	std::condition_variable completionCondition;
};

class Renderer
{
public:
	Renderer(sf::RenderWindow* window);
	~Renderer();

	void Init();

	void Draw3DView(Player* player, Map* map, std::vector<std::shared_ptr<Sprite>>* sprites);
private:
	sf::RenderWindow* window;
	sf::Texture floorTexture;
	sf::Sprite floorSprite;
	uint8_t* screenPixels;

	sf::VertexArray walls{ sf::Lines };
	sf::VertexArray spriteColumns{ sf::Lines };
	float* distanceBuffer;

	ThreadPool threads;

	void DrawFloor(sf::Vector2f& rayDirLeft, sf::Vector2f& rayDirRight, sf::Vector2f& rayPos, 
		Player* player, Map* map, int startH, int endH);
	void DrawSprite(sf::Vector2f& pDirection, sf::Vector2f& cameraPlane, Player* player,
		std::vector<std::shared_ptr<Sprite>>* sprites);
};

#endif // !RENDERER
