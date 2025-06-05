#include "Renderer.h"
#include <SFML/Graphics/RectangleShape.hpp>

ThreadPool::ThreadPool(int thread_count) : 
	active_tasks{ 0 }, stop{false}
{
	for (size_t i = 0; i < thread_count; ++i)
	{
		workers.emplace_back([this]() {
			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					condition.wait(lock, [this]() { return stop || !tasks.empty(); });
					if (stop && tasks.empty()) return;
					task = std::move(tasks.front());
					tasks.pop();
					++active_tasks;
				}
				task();
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					--active_tasks;
					if (active_tasks == 0) { completion_condition.notify_all(); }
				}
			}});
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}

	condition.notify_all();

	for (auto& worker : workers)
	{
		if (worker.joinable()) worker.join();
	}
}

void ThreadPool::AddTask(std::function<void()>&& task)
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.push(std::move(task));
	}

	condition.notify_one();
}

void ThreadPool::WaitAll()
{
	std::unique_lock<std::mutex> lock(queue_mutex);
	completion_condition.wait(lock, [this]() { return active_tasks == 0; });
}

int ThreadPool::GetThreadCount() { return workers.size(); }

Renderer::Renderer(sf::RenderWindow* _window, Player* player) :
	window{ _window }, player{ player }, threads {
	std::max((int)std::thread::hardware_concurrency() - 2, 3) }
{
	Init();
}

Renderer::~Renderer()
{
	delete[] distance_buffer;
	delete[] screen_pixels;
}

void Renderer::Init()
{
	floor_texture.create(kScreenWight, kScreenHeight);
	floor_sprite.setTexture(floor_texture);
	screen_pixels = new uint8_t[kScreenHeight * kScreenWight * 4]();
	distance_buffer = new float[kScreenWight + 1] {};
}

void Renderer::Draw3DView(Map* map, std::vector<std::shared_ptr<Sprite>>* sprites)
{
	//StaticCalculations
	float player_angle_radians = player->enemy->map_sprite.angle * kPI / 180.0f;
	sf::Vector2f player_direction{ cosf(player_angle_radians), sinf(player_angle_radians) };
	sf::Vector2f camera_plane = sf::Vector2f(-player_direction.y, player_direction.x) * kAspect;
	sf::Vector2f ray_position = player->enemy->map_sprite.position;

	//FloorPart
	sf::Vector2f ray_direction_left{ player_direction - camera_plane },
		rayDirRight{ player_direction + camera_plane };
	int thread_count = threads.GetThreadCount() - 1;
	for (int cnt = 0; cnt < thread_count; cnt++)
	{
		threads.AddTask([&, cnt]()
			{
				int start = (int)(((float)kScreenHeight / thread_count * cnt));
				int end = (int)(((float)kScreenHeight / thread_count * (cnt + 1)));
				DrawFloor(ray_direction_left, rayDirRight, ray_position, map, start, end);
			});
	}

	//SpritePart
	threads.AddTask([&]() {
		std::sort(sprites->begin(), sprites->end(), 
		[&](const std::shared_ptr<Sprite> a, const std::shared_ptr<Sprite> b)
			{
				return COMPARER(a->map_sprite.position, b->map_sprite.position, player->enemy->map_sprite.position);
			});});

	//SkyPart
	sf::Vector2u sky_texture_size = Resources::sky_textures.getSize();
	int texture_offset_x = (int)(player->enemy->map_sprite.angle / 90.0f * sky_texture_size.x);
	while (texture_offset_x < 0)
	{
		texture_offset_x += sky_texture_size.x;
	}

	sf::Vertex sky[] = {
		sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(texture_offset_x, -player->pitch)),
		sf::Vertex(sf::Vector2f(0.0f, kScreenHeight), sf::Vector2f(texture_offset_x, sky_texture_size.y - player->pitch)),
		sf::Vertex(sf::Vector2f(kScreenWight, kScreenHeight),
		sf::Vector2f(texture_offset_x + sky_texture_size.x, sky_texture_size.y - player->pitch)),
		sf::Vertex(sf::Vector2f(kScreenWight, 0), sf::Vector2f(texture_offset_x + sky_texture_size.x, -player->pitch))
	};

	window->draw(sky, 4, sf::Quads, sf::RenderStates(&Resources::sky_textures));
	//NewAlgoritmPart
	sf::Vector2f ray_direction{};
	for (int i = 0; i <= kScreenWight; i++)
	{
		float camera_x = i * 2.0f / kScreenWight - 1.0f;
		ray_direction = player_direction + camera_plane * camera_x;

		RayHit hit = Raycast(map, ray_position, ray_direction);

		if (hit.cell)
		{
			float wall_height = kScreenHeight / hit.perpWallDist;
			float wall_start = (kScreenHeight - wall_height) / 2.0f + player->pitch + player->pos_z / hit.perpWallDist;
			float wall_end = (kScreenHeight + wall_height) / 2.0f + player->pitch + player->pos_z / hit.perpWallDist;


			float wall_x = hit.isHitVert ? ray_position.x + hit.perpWallDist * ray_direction.x :
				ray_position.y + hit.perpWallDist * ray_direction.y;

			wall_x -= floor(wall_x);
			float texture_x = wall_x * kTextureSize;

			float brightnes = 1.0f - (hit.perpWallDist / kBrightnestDist);
			if (brightnes < 0.11f) brightnes = 0.11f;
			if (hit.isHitVert) brightnes -= 0.09f;
			sf::Color color_shade(255 * brightnes, 255 * brightnes, 255 * brightnes);

			walls.append(sf::Vertex(sf::Vector2f((float)i, wall_start), color_shade,
				sf::Vector2f(texture_x + (hit.cell - 1) % (int)kTextureCount * kTextureSize,
					(hit.cell - 1) / (int)kTextureCount * kTextureSize)));
			walls.append(sf::Vertex(sf::Vector2f((float)i, wall_end + 1), color_shade,
				sf::Vector2f(texture_x + (hit.cell - 1) % (int)kTextureCount * kTextureSize,
					kTextureSize + (hit.cell - 1) / (int)kTextureCount * kTextureSize)));

			distance_buffer[i] = hit.perpWallDist;
		}
	}

	//ThreadPart
	threads.WaitAll();

	//DrawPart
	floor_texture.update(screen_pixels);
	window->draw(floor_sprite);

	sf::RenderStates states{ &Resources::textures };
	window->draw(walls, states);

	DrawSprite(player_direction, camera_plane, sprites);

	sprite_columns.clear();
	walls.clear();
}

void Renderer::DrawSprite(const sf::Vector2f& player_direction, const sf::Vector2f& camera_plane,
	std::vector<std::shared_ptr<Sprite>>* sprites)
{
	float invert_det = 1.0f / (camera_plane.x * player_direction.y - camera_plane.y * player_direction.x);
	for (auto sp : *sprites)
	{
		if (sp->sprite_def.texture < 0) continue;
		sf::Vector2f sprite_position = sp->map_sprite.position - player->enemy->map_sprite.position;
		float sprite_dist = sqrt(SQUARE(sprite_position.x) + SQUARE(sprite_position.y));
		float brightnes = 1 - sprite_dist / kBrightnestDist;
		auto sprite_data = sp->GetTextIndex();
		if (brightnes < 0.1f) brightnes = 0.1f;
		sf::Color color_shade(255 * brightnes, 255 * brightnes, 255 * brightnes);
		if (sprite_data.second)
		{
			if (color_shade.r * 1.5f > 255) color_shade.r = 255;
			color_shade.g /= 2;
			color_shade.b /= 2;
		}

		//transform sprite with the inverse camera matrix
		// [ planeX   dirX ] -1    [x]                                     [ dirY      -dirX ]   [x]
		// [               ]     * [ ]   =  1/(planeX*dirY-dirX*planeY) *  [                 ] * [ ]
		// [ planeY   dirY ]       [y]                                     [ -planeY  planeX ]   [y]

		sf::Vector2f transforme(player_direction.y * sprite_position.x - player_direction.x * sprite_position.y,
			-camera_plane.y * sprite_position.x + camera_plane.x * sprite_position.y);
		transforme *= invert_det;

		int screen_x = kScreenWight / 2.0f * (1 + transforme.x / transforme.y);
		int sprite_size = abs(kScreenHeight / transforme.y);

		int draw_start_x = -sprite_size / 2 + screen_x;
		int draw_end_x = sprite_size / 2 + screen_x;

		float delta_rotate_text = 0.0f;
		if (sp->sprite_def.isDirectional)
		{
			sf::Vector2f dir = sprite_position;
			float len = sqrt(dir.x * dir.x + dir.y * dir.y);
			dir /= len;

			float angle = sp->map_sprite.angle - atan2(dir.y, dir.x) * 180.0f / kPI + 180.0f;
			angle = round(angle / 45.0f) * 45.0f;
			angle = fmod(angle, 360.0f);
			if (angle < 0.0f)
			{
				angle += 360.0f;
			}
			delta_rotate_text = sp->texture_size * (angle / 45.0f);
		}

		int sprite_start = -sprite_size * sp->sprite_def.size / 2 + screen_x,
			spriteEnd = sprite_size * sp->sprite_def.size / 2 + screen_x;

		for (int i = std::max(draw_start_x, 0); i < std::min(draw_end_x, (int)kScreenWight - 1); i++)
		{
			if (transforme.y > 0 && transforme.y < distance_buffer[i])
			{
				float text_x = (i - draw_start_x) * sp->texture_size / sprite_size;
				sf::Vector2f text_start(text_x + delta_rotate_text, sprite_data.first * sp->texture_size);
				sf::Vector2f text_end(text_x + delta_rotate_text, (sprite_data.first + 1) * sp->texture_size);

				sf::Vector2f vert_start(i, -sprite_size / 2.0f + kScreenHeight / 2.0f + player->pitch + player->pos_z / transforme.y);
				sf::Vector2f vert_end(i, sprite_size / 2.0f + kScreenHeight / 2.0f + player->pitch + player->pos_z / transforme.y);

				sprite_columns.append(sf::Vertex(vert_start, color_shade, text_start));
				sprite_columns.append(sf::Vertex(vert_end, color_shade, text_end));
			}
		}
		sf::RenderStates state(sp->texture);
		window->draw(sprite_columns, state);
		sprite_columns.clear();
	}
}

void Renderer::DrawFloor(const sf::Vector2f& ray_direction_left, const sf::Vector2f& ray_direction_right, 
	const sf::Vector2f& ray_position, Map* map, int start_height, int end_height)
{
	for (int y = start_height; y < end_height; y++)
	{
		bool is_floor = y > kScreenHeight / 2 + player->pitch;
		int p = is_floor ? (y - kScreenHeight / 2 - player->pitch) : (kScreenHeight / 2 - y + player->pitch);
		float row_dist = is_floor ? (kCameraZ + player->pos_z) / p : (kCameraZ - player->pos_z) / p;
		sf::Vector2f floor_step = row_dist * (ray_direction_right - ray_direction_left) / (float)kScreenWight;
		sf::Vector2f floor = ray_position + row_dist * ray_direction_left;
		float brightnes = 1 - row_dist / kBrightnestDist;
		if (brightnes < 0.08f) brightnes = 0.08f;

		for (int x = 0; x < kScreenWight; x++)
		{
			sf::Vector2i cell{ floor };
			sf::Vector2i text_coords{ (float)kTextureSize * (floor - (sf::Vector2f)cell) };
			text_coords.x &= kTextureSize - 1;
			text_coords.y &= kTextureSize - 1;

			int floor_text = map->GetOnGrid(cell.x, cell.y, kFloorLaye);
			int celling_text = map->GetOnGrid(cell.x, cell.y, kCellLayer);

			sf::Color color;
			if (is_floor)
			{
				color = floor_text == 0 ? sf::Color(0, 0, 0, 0) :
					Resources::texture_image.getPixel((floor_text - 1) % kTextureCount * kTextureSize + text_coords.x,
						(floor_text - 1) / kTextureCount * kTextureSize + text_coords.y);
			}
			else
			{
				color = celling_text == 0 ? sf::Color(0, 0, 0, 0) :
					Resources::texture_image.getPixel((celling_text - 1) % (int)kTextureCount * kTextureSize + text_coords.x,
						(celling_text - 1) / (int)kTextureCount * kTextureSize + text_coords.y);
			}
			screen_pixels[(x + y * (int)kScreenWight) * 4 + 0] = color.r * brightnes;
			screen_pixels[(x + y * (int)kScreenWight) * 4 + 1] = color.g * brightnes;
			screen_pixels[(x + y * (int)kScreenWight) * 4 + 2] = color.b * brightnes;
			screen_pixels[(x + y * (int)kScreenWight) * 4 + 3] = color.a;

			floor += floor_step;
		}
	}
}