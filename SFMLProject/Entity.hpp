#pragma once
#include <SFML/Graphics.hpp>
#include "SceneNode.hpp"
#include <cassert>

class Entity: public SceneNode {
private:
	sf::Vector2f mVelocity;
	int mHitpoints;

protected:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);

public:
	explicit Entity(int hitpoints);

	void setVelocity(sf::Vector2f velocity);
	void setVelocity(float vx, float vy);
	void accelerate(sf::Vector2f velocity);
	void accelerate(float vx, float vy);
	sf::Vector2f getVelocity() const;
	int getHitpoints() const;
	void repair(int points);
	void damage(int points);
	void destroy();
	virtual void remove();
	bool isDestroyed() const;
};