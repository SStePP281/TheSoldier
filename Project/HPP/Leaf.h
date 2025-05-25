#ifndef LEAF
#define LEAF

#include "Randomizer.h"
#include "CONST.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

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