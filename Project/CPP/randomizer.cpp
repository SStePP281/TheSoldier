#include "randomizer.h"

float Random::BitRandom() {
	return IntRandom(0, 10000) / 10000.0f;
}

int Random::IntRandom(int start, int end) {
	if (start > end)
		throw "logic_error";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(start, end);
	return distr(gen);
}

std::set<sf::Vector2i, Vector2iCompare> Random::UniquePoints(sf::IntRect zone, int count) {
	std::set<sf::Vector2i, Vector2iCompare> points;

	while (points.size() < count) {
		points.insert({ IntRandom(zone.left, (zone.left + zone.width)),
						IntRandom(zone.top, (zone.top + zone.height))});
	}
	
	return points;
}
