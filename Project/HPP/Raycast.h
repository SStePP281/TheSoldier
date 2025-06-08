#ifndef RAYCAST
#define RAYCAST

#include "map.h"
#include "const.h"

RayHit Raycast(const Map* const map, const sf::Vector2f& position, const sf::Vector2f& direction,
	bool check_sprite = false, Sprite* ignore = nullptr, int max_dist = 64, float pitch = 0.0f);

#endif // !RAYCAST
