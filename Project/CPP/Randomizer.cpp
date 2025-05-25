#include "Randomizer.h"

float Random::bitRandom()
{
	return intRandom(0, 10000) / 10000.0f;
}
int Random::intRandom(int a, int b)
{
	if (a > b)
		throw "logic_error";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(a, b);
	return distr(gen);
}

std::set<sf::Vector2i, Vector2iCompare> Random::uniquePoints(sf::IntRect zone, int count)
{
	std::set<sf::Vector2i, Vector2iCompare> points;

	while (points.size() < count)
	{
		points.insert({ intRandom(zone.left, (zone.left + zone.width)),
						intRandom(zone.top, (zone.top + zone.height))});
	}
	
	return points;
}
