#include "Raycast.h"

RayHit raycast(Map* map, sf::Vector2f& pos, sf::Vector2f& dir,
	bool checkSprite, Sprite* ignore, int maxDist, float pitch)
{
	float origin[2] = { pos.x, pos.y };
	float dir_inv_x = dir.x == 0 ? INFINITY : 1.0f / dir.x;
	float dir_inv_y = dir.y == 0 ? INFINITY : 1.0f / dir.y;
	float dir_inv[2] = { dir_inv_x, dir_inv_y };
	sf::Vector2f deltaDist{ abs(dir_inv[0]), abs(dir_inv[1]) };
	sf::Vector2i mapPos{ pos };
	sf::Vector2i step;
	sf::Vector2f sideDist;

	if (dir.x < 0)
	{
		step.x = -1;
		sideDist.x = (-mapPos.x + pos.x) * deltaDist.x;
	}
	else
	{
		step.x = 1;
		sideDist.x = (mapPos.x - pos.x + 1.0f) * deltaDist.x;
	}

	if (dir.y < 0)
	{
		step.y = -1;
		sideDist.y = (-mapPos.y + pos.y) * deltaDist.y;
	}
	else
	{
		step.y = 1;
		sideDist.y = (mapPos.y - pos.y + 1.0f) * deltaDist.y;
	}

	int hit{};
	bool isVertHit{};
	Sprite* hitSprite{};
	for (int depth = 0; !hitSprite && !hit && depth < maxDist; depth++)
	{
		if (sideDist.x < sideDist.y)
		{
			sideDist.x += deltaDist.x;
			mapPos.x += step.x;
			isVertHit = false;
		}
		else
		{
			sideDist.y += deltaDist.y;
			mapPos.y += step.y;
			isVertHit = true;
		}

		hit = map->GetOnGrid(mapPos.x, mapPos.y, WALL_LAYER);

		if (hit || !checkSprite) continue;

		auto block = map->getBlockMap(mapPos);
		for (auto sprite : block)
		{
			if (sprite == ignore) continue;

			sf::Vector2f halfSize = { sprite->spDef.size / 2.0f, sprite->spDef.size / 2.0f };
			sf::Vector2f start = sprite->spMap.position - halfSize;
			sf::Vector2f end = sprite->spMap.position + halfSize;

			float min[2] = { start.x, start.y };
			float max[2] = { end.x, end.y };

			float tmin = 0.0f, tmax = INFINITY;

			for (int d = 0; d < 2; d++)
			{
				float t1 = (min[d] - origin[d]) * dir_inv[d];
				float t2 = (max[d] - origin[d]) * dir_inv[d];

				tmin = fmax(tmin, fmin(tmax, fmin(t1, t2)));
				tmax = fmin(tmax, fmax(tmin, fmax(t1, t2)));
			}

			if (tmin < tmax)
			{
				int poss = pitch * sqrt(GETDIST(pos, sprite->spMap.position)) / 3;
				if (poss - 20 < sprite->textSize && poss + 20 > -sprite->textSize)
				{
					hitSprite = sprite;
				}
			}
		}
	}

	return RayHit{ hit, mapPos, isVertHit,
	isVertHit ? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x, hitSprite };
}