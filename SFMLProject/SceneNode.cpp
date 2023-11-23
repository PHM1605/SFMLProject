#include "SceneNode.hpp"
#include <iostream>

SceneNode::SceneNode(Category::Type category):
	mChildren(), 
	mParent(nullptr),
	mDefaultCategory(category)
{

}

void SceneNode::attachChild(Ptr child) {
	child->mParent = this;
	mChildren.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node) {
	auto found = std::find_if(mChildren.begin(), mChildren.end(), [&](Ptr& p) -> bool { return p.get() == &node;  });
	assert(found != mChildren.end());
	Ptr result = std::move(*found);
	result->mParent = nullptr;
	mChildren.erase(found);
	return result;
}

void SceneNode::update(sf::Time dt, CommandQueue& commands) {
	updateCurrent(dt, commands);
	updateChildren(dt, commands);
}

void SceneNode::updateCurrent(sf::Time, CommandQueue& commands) {} // to be overwritten

void SceneNode::updateChildren(sf::Time dt, CommandQueue& commands) {
	for (Ptr& child : mChildren) {
		child->update(dt, commands);
	}
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	drawCurrent(target, states);
	drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget&, sf::RenderStates) const {}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const {
	for (const Ptr& child : mChildren) {
		child->draw(target, states);
	}
}

void SceneNode::drawBoundingRect(sf::RenderTarget& target, sf::RenderStates) const {
	sf::FloatRect rect = getBoundingRect();
	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(rect.left, rect.top));
	shape.setSize(sf::Vector2f(rect.width, rect.height));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1.f);
	target.draw(shape);
}

sf::Vector2f SceneNode::getWorldPosition() const {
	return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::getWorldTransform() const {
	sf::Transform transform = sf::Transform::Identity;
	for (const SceneNode* node = this; node != nullptr; node = node->mParent) {
		transform = node->getTransform() * transform;
	}
	return transform;
}

unsigned int SceneNode::getCategory() const {
	return mDefaultCategory;
}

void SceneNode::onCommand(const Command& command, sf::Time dt) {
	if (command.category & getCategory())
		command.action(*this, dt);
	for (Ptr& child : mChildren) {
		child->onCommand(command, dt);
	}
}

sf::FloatRect SceneNode::getBoundingRect() const {
	return sf::FloatRect();
}

bool SceneNode::isMarkedForRemoval() const {
	return isDestroyed();
}

// By default, SceneNode is not destroyed
bool SceneNode::isDestroyed() const {
	return false;
}

float distance(const SceneNode& lhs, const SceneNode& rhs) {
	return length(lhs.getWorldPosition() - rhs.getWorldPosition());
}