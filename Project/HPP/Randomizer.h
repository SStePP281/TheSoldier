#ifndef RANDOM
#define RANDOM

#include "sfml_extantion.h"
#include <random>
#include <set>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class Random {
public:
	static float BitRandom();
	static int IntRandom(int start, int end);
	static std::set<sf::Vector2i, Vector2iCompare> UniquePoints(sf::IntRect zone, int count);
};

#endif // !RANDOM
