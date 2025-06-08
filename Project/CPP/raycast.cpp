#include "raycast.h"

RayHit Raycast(const Map* const  map, const sf::Vector2f& position, const sf::Vector2f& direction,
	bool check_sprite, Sprite* ignore, int max_dist, float pitch) {
	float origin[2] = { position.x, position.y };
	float dir_inv_x = direction.x == 0 ? INFINITY : 1.0f / direction.x;
	float dir_inv_y = direction.y == 0 ? INFINITY : 1.0f / direction.y;
	float dir_inv[2] = { dir_inv_x, dir_inv_y };
	sf::Vector2f delta_dist{ abs(dir_inv[0]), abs(dir_inv[1]) };
	sf::Vector2i map_pos{ position };
	sf::Vector2i step;
	sf::Vector2f side_dist;

	if (direction.x < 0) {
		step.x = -1;
		side_dist.x = (-map_pos.x + position.x) * delta_dist.x;
	} else {
		step.x = 1;
		side_dist.x = (map_pos.x - position.x + 1.0f) * delta_dist.x;
	}

	if (direction.y < 0) {
		step.y = -1;
		side_dist.y = (-map_pos.y + position.y) * delta_dist.y;
	} else {
		step.y = 1;
		side_dist.y = (map_pos.y - position.y + 1.0f) * delta_dist.y;
	}

	int hit{};
	bool is_vertical_hit{};
	Sprite* hit_sprite{};

	for (int depth = 0; !hit_sprite && !hit && depth < max_dist; depth++) {
		if (side_dist.x < side_dist.y) {
			side_dist.x += delta_dist.x;
			map_pos.x += step.x;
			is_vertical_hit = false;
		} else {
			side_dist.y += delta_dist.y;
			map_pos.y += step.y;
			is_vertical_hit = true;
		}

		hit = map->GetOnGrid(map_pos.x, map_pos.y, kWallLayer);

		if (hit || !check_sprite) continue;

		auto block = map->GetBlockMap(map_pos);
		for (const auto& sprite : block) {
			if (sprite == ignore) continue;

			sf::Vector2f half_size = { sprite->GetSize() / 2.0f, sprite->GetSize() / 2.0f };
			sf::Vector2f start = sprite->GetPosition() - half_size;
			sf::Vector2f end = sprite->GetPosition() + half_size;

			float min[2] = { start.x, start.y };
			float max[2] = { end.x, end.y };

			float tmin = 0.0f, tmax = INFINITY;

			for (int d = 0; d < 2; d++) {
				float t1 = (min[d] - origin[d]) * dir_inv[d];
				float t2 = (max[d] - origin[d]) * dir_inv[d];

				tmin = fmax(tmin, fmin(tmax, fmin(t1, t2)));
				tmax = fmin(tmax, fmax(tmin, fmax(t1, t2)));
			}

			if (tmin < tmax) {
				int poss = pitch * sqrt(GETDIST(position, sprite->GetPosition())) / 3;
				if (poss - 20 < sprite->GetTextureSize() && poss + 20 > -sprite->GetTextureSize()) {
					hit_sprite = sprite;
				}
			}
		}
	}

	return RayHit{ hit, map_pos, is_vertical_hit,
	is_vertical_hit ? side_dist.y - delta_dist.y : side_dist.x - delta_dist.x, hit_sprite };
}
