#ifndef RENDERER
#define RENDERER

#include "player.h"
#include "map.h"
#include "sprite.h"
#include "raycast.h"
#include "const.h"
#include "resources.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <thread>
#include <queue>

class ThreadPool {
public:
	ThreadPool(int thread_count);
	~ThreadPool();
	void AddTask(std::function<void()>&& task);
	void WaitAll();
	int GetThreadCount();
private:
	std::vector<std::thread> workers_;
	std::queue<std::function<void()>> tasks_;
	std::mutex queue_mutex;
	std::condition_variable condition_;
	bool stop_;
	size_t active_tasks_;
	std::condition_variable completion_condition_;
};

class Renderer {
public:
	Renderer(sf::RenderWindow* window, const Player* const player);
	~Renderer();

	void Init();

	void Draw3DView(const Map* const map, std::vector<std::shared_ptr<Sprite>>* const  sprites);
private:
	void DrawFloor(const sf::Vector2f& ray_direction_left, const sf::Vector2f& ray_direction_right, 
		const sf::Vector2f& ray_proisition, const Map* const map, int start_height, int end_height);
	void DrawSprite(const sf::Vector2f& player_direction, const sf::Vector2f& camera_plane, std::vector<std::shared_ptr<Sprite>>* const sprites);

	sf::RenderWindow* window_;
	sf::Texture floor_texture_;
	sf::Sprite floor_sprite_;
	uint8_t* screen_pixels_;
	const Player* const player_;
	sf::VertexArray walls_{ sf::Lines };
	sf::VertexArray sprite_columns_{ sf::Lines };
	float* distance_buffer_;
	ThreadPool threads_;
};

#endif // !RENDERER
