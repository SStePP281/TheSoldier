#ifndef RENDERER
#define RENDERER

#include "Player.h"
#include "Map.h"
#include "Sprite.h"
#include "Raycast.h"
#include "CONST.h"
#include "Resources.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <thread>
#include <queue>

class ThreadPool
{
public:
	ThreadPool(int thread_count);
	~ThreadPool();
	void AddTask(std::function<void()>&& task);
	void WaitAll();
	int GetThreadCount();
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
	size_t active_tasks;
	std::condition_variable completion_condition;
};

class Renderer
{
public:
	Renderer(sf::RenderWindow* window, Player* player);
	~Renderer();

	void Init();

	void Draw3DView(Map* map, std::vector<std::shared_ptr<Sprite>>* sprites);
private:
	void DrawFloor(const sf::Vector2f& ray_direction_left, const sf::Vector2f& ray_direction_right, 
		const sf::Vector2f& ray_proisition, Map* map, int start_height, int end_height);
	void DrawSprite(const sf::Vector2f& player_direction, const sf::Vector2f& camera_plane, std::vector<std::shared_ptr<Sprite>>* sprites);

	sf::RenderWindow* window;
	sf::Texture floor_texture;
	sf::Sprite floor_sprite;
	uint8_t* screen_pixels;
	Player* player;
	sf::VertexArray walls{ sf::Lines };
	sf::VertexArray sprite_columns{ sf::Lines };
	float* distance_buffer;
	ThreadPool threads;
};

#endif // !RENDERER
