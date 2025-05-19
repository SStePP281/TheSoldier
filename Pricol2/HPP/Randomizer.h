#pragma once
#ifndef RANDOM
#define RANDOM

#include "sfmlExtantion.h"
#include <random>
#include <set>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class Random
{
public:
	static float bitRandom();
	static int intRandom(int, int);
	static std::set<sf::Vector2i, Vector2iCompare> uniquePoints(sf::IntRect zone, int count);
};
#endif // !RANDOM