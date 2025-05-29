#pragma once
#ifndef RAYCAST
#define RAYCAST

#include "Map.h"
#include "CONST.h"

struct RayHit
{
	int cell;
	sf::Vector2i mapPos;
	bool isHitVert;
	float perpWallDist;

	Sprite* sprite;
};

RayHit raycast(Map* map, const sf::Vector2f& pos, const sf::Vector2f& dir,
	bool checkSprite = false, Sprite* ignore = nullptr, int maxDist = 64, float pitch = 0.0f);

#endif // !RAYCAST