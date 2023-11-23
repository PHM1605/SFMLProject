#pragma once
#include <cassert>
#include <functional>
#include <SFML/Graphics.hpp>
#include "Category.hpp"

class SceneNode;

struct Command {
	typedef std::function<void(SceneNode&, sf::Time)> Action;
	Command();
	Action action;
	unsigned int category;
};

// casting GameObject e.g. Aircraft to SceneNode to be put on Queue
template <typename GameObject, typename Function>
Command::Action derivedAction(Function fn) {
	return [=](SceneNode& node, sf::Time dt) {
		assert(dynamic_cast<GameObject*>(&node) != nullptr);
		fn(static_cast<GameObject&>(node), dt);
	};
}