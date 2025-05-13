#ifndef LEAF
#define LEAF

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>
#include "Randomizer.h"

constexpr int MIN_LEAF_SIZE = 10, MAX_LEAF_SIZE = 20;

class Leaf
{
private:
	std::vector<Leaf*> allChild;
public:
	sf::IntRect leafData;
	bool isRoom;
	Leaf* leftChild;
	Leaf* rightChild;

	Leaf(sf::Vector2i, sf::Vector2i);
	Leaf() = default;
	~Leaf();

	std::vector<Leaf*> getRoom();
	bool split();
	void findRoom();
	std::vector<Leaf*> getAllChild();
};

#endif // !LEAF