#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <set>
#include <cassert>
#include "Category.hpp"
#include "Command.hpp"
#include "CommandQueue.hpp"
#include  "Utility.hpp"
#include <set>
#include <algorithm>

class SceneNode: public sf::Drawable, public sf::Transformable, sf::NonCopyable {
public:
	typedef std::unique_ptr<SceneNode> Ptr; 
	typedef std::pair<SceneNode*, SceneNode*> Pair;
public:
	SceneNode(Category::Type category = Category::None);
	void attachChild(Ptr child);
	Ptr detachChild(const SceneNode& node);
	void update(sf::Time dt, CommandQueue& commands);
	sf::Vector2f getWorldPosition() const;
	sf::Transform getWorldTransform() const;
	void onCommand(const Command& command, sf::Time dt);
	virtual unsigned int getCategory() const;
	void checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);
	void removeWrecks();
	virtual sf::FloatRect getBoundingRect() const;
	virtual bool isMarkedForRemoval() const;
	virtual bool isDestroyed() const;
protected:
	std::vector<Ptr> mChildren;
	SceneNode* mParent;
	Category::Type mDefaultCategory;
private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateChildren(sf::Time dt, CommandQueue& commands);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const;
};

bool collision(const SceneNode& lhs, const SceneNode& rhs);
float distance(const SceneNode& lhs, const SceneNode& rhs);