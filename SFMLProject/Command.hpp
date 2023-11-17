#pragma once
#include <functional>
#include <SFML/Graphics.hpp>
#include "Category.hpp"

class SceneNode;

struct Command {
	Command();
	std::function<void(SceneNode&, sf::Time)> action;
	unsigned int category;
};
