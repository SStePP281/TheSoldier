#include "leaf.h"

Leaf::Leaf(sf::Vector2i position, sf::Vector2i size) : 
	leaf_data{ sf::IntRect(position.x, position.y, size.x, size.y) },
	left_child{nullptr}, right_child{nullptr}, is_room{false} { }

Leaf::~Leaf() {
	delete left_child;
	delete right_child;
}

const std::vector<Leaf*>& Leaf::GetAllChild() {
	if (left_child) {
		if (left_child->is_room) {
			all_child_.push_back(left_child);
		} else {
			for (const auto& d : left_child->GetAllChild()) {
				all_child_.push_back(d);
			}
		}
	}

	if (right_child) {
		if (right_child->is_room) {
			all_child_.push_back(right_child);
		} else {
			for (const auto& d : right_child->GetAllChild()) {
				all_child_.push_back(d);
			}
		}
	}

	return all_child_;
}

const std::vector<Leaf*> Leaf::GetRoom() {
	std::vector<Leaf*> rooms;

	for (const auto& l : all_child_) {
		if (l->is_room) {
			rooms.push_back(l);
		}
	}

	return rooms;
}

bool Leaf::Split() {
	if (left_child || right_child) { return false; }

	bool is_horizontal_rez;
	if (leaf_data.width > leaf_data.height && leaf_data.width / (float)leaf_data.height >= 1.25f) {
		is_horizontal_rez = false;
	} else if (leaf_data.height > leaf_data.width && leaf_data.height / (float)leaf_data.width >= 1.25f) {
		is_horizontal_rez = true;
	} else {
		is_horizontal_rez = Random::BitRandom() > 0.5f;
	}

	int max = (is_horizontal_rez ? leaf_data.height : leaf_data.width) - kMinLeafSize;
	if (max <= kMinLeafSize) { return false; }
	int value_rez = Random::IntRandom(kMinLeafSize, max);

	if (is_horizontal_rez) {
		left_child = new Leaf({ leaf_data.left, leaf_data.top }, { leaf_data.width, value_rez });
		right_child = new Leaf({ leaf_data.left, leaf_data.top + value_rez }, { leaf_data.width, leaf_data.height - value_rez });
	} else {
		left_child = new Leaf({ leaf_data.left, leaf_data.top }, { value_rez, leaf_data.height });
		right_child = new Leaf({ leaf_data.left + value_rez, leaf_data.top }, { leaf_data.width - value_rez, leaf_data.height });
	}

	return true;
}

void Leaf::FindRoom() {
	if (left_child && right_child) {
		left_child->FindRoom();
		right_child->FindRoom();
	} else {
		is_room = true;
	}
}
