#ifndef LEAF
#define LEAF

#include "randomizer.h"
#include "const.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Leaf {
public:
	Leaf(sf::Vector2i position, sf::Vector2i size);
	Leaf() = default;
	~Leaf();

	const std::vector<Leaf*> GetRoom();
	bool Split();
	void FindRoom();
	const std::vector<Leaf*>& GetAllChild();

	sf::IntRect leaf_data;
	bool is_room;
	Leaf* left_child;
	Leaf* right_child;
private:
	std::vector<Leaf*> all_child_;
};

#endif // !LEAF
