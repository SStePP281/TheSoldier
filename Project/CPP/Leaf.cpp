#include "Leaf.h"

Leaf::Leaf(sf::Vector2i pos, sf::Vector2i size)
{
	leftChild = nullptr;
	rightChild = nullptr;
	leafData = sf::IntRect(pos.x, pos.y, size.x, size.y);
	isRoom = false;
}

Leaf::~Leaf()
{
	delete leftChild;
	delete rightChild;
}

std::vector<Leaf*> Leaf::getAllChild()
{
	if (leftChild)
	{
		if (leftChild->isRoom)
		{
			allChild.push_back(leftChild);
		}
		else
		{
			auto data = leftChild->getAllChild();
			for (auto d : data)
			{
				allChild.push_back(d);
			}
		}
	}

	if (rightChild)
	{
		if (rightChild->isRoom)
		{
			allChild.push_back(rightChild);
		}
		else
		{
			auto data = rightChild->getAllChild();
			for (auto d : data)
			{
				allChild.push_back(d);
			}
		}
	}
	return allChild;
}

std::vector<Leaf*> Leaf::getRoom()
{
	std::vector<Leaf*> rooms;

	for (auto l : allChild)
	{
		if (l->isRoom)
		{
			rooms.push_back(l);
		}
	}

	return rooms;
}

bool Leaf::split()
{
	if (leftChild || rightChild) { return false; }

	bool isHorRez;
	if (leafData.width > leafData.height && leafData.width / (float)leafData.height >= 1.25f) {
		isHorRez = false;
	}
	else if (leafData.height > leafData.width && leafData.height / (float)leafData.width >= 1.25f) {
		isHorRez = true;
	}
	else
	{
		isHorRez = Random::bitRandom() > 0.5f;
	}

	int max = (isHorRez ? leafData.height : leafData.width) - MIN_LEAF_SIZE;
	if (max <= MIN_LEAF_SIZE) { return false; }
	int valRez = Random::intRandom(MIN_LEAF_SIZE, max);

	if (isHorRez)
	{
		leftChild = new Leaf({ leafData.left, leafData.top }, { leafData.width, valRez });
		rightChild = new Leaf({ leafData.left, leafData.top + valRez }, { leafData.width, leafData.height - valRez });
	}
	else
	{
		leftChild = new Leaf({ leafData.left, leafData.top }, { valRez, leafData.height });
		rightChild = new Leaf({ leafData.left + valRez, leafData.top }, { leafData.width - valRez, leafData.height });
	}
	return true;
}

void Leaf::findRoom()
{
	if (leftChild && rightChild)
	{
		leftChild->findRoom();
		rightChild->findRoom();
	}
	else
	{
		isRoom = true;
	}
}
