#pragma once
#include "Entity.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "DataTables.hpp"
#include "TextNode.hpp"
#include "CommandQueue.hpp"

class Aircraft : public Entity {
public:
	enum Type { Eagle, Raptor, Avenger, TypeCount};
public: 
	Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);
	virtual unsigned int getCategory() const;
	//virtual sf::FloatRect getBoundingRect() const;
	float getMaxSpeed() const;
private:
	Type mType;
	sf::Sprite mSprite;
	float mTravelledDistance;
	std::size_t mDirectionIndex;
	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateMovementPattern(sf::Time dt);
	void updateTexts();
};