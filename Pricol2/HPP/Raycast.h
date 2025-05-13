#pragma once
#ifndef RAYCAST
#define RAYCAST

#define SQUARE(a) ((a) * (a))
#define GETDIST(a,b) (SQUARE(a.x - b.x) + SQUARE(a.y - b.y))
#define COMPARER(a, b, c) GETDIST(a, c) > GETDIST(b, c) ? true : false

#include "Map.h"

struct RayHit
{
	int cell;
	sf::Vector2i mapPos;
	bool isHitVert;
	float perpWallDist;

	Sprite* sprite;
};

RayHit raycast(Map* map, sf::Vector2f& pos, sf::Vector2f& dir,
	bool checkSprite = false, Sprite* ignore = nullptr, int maxDist = 64, float pitch = 0.0f);

#endif // !RAYCAST