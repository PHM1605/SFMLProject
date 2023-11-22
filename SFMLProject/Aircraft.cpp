#include "Aircraft.hpp"

namespace {
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts) :
	Entity(Table[type].hitpoints),
	mType(type),
	mSprite(textures.get(Table[type].texture)), // convert Aircraft::Type e.g. 'Eagle' enum to Textures::ID enum 
	mTravelledDistance(0.f),
	mDirectionIndex(0),
	mHealthDisplay(nullptr)
{	
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));
	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands) {
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
	updateTexts();
}

unsigned int Aircraft::getCategory() const {
	switch (mType) {
	case Eagle:
		return Category::PlayerAircraft;
	default:
		return Category::EnemyAircraft;
	}
}

void Aircraft::updateTexts() {
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation()); // set health text not rotating when plane rotates
}

void Aircraft::updateMovementPattern(sf::Time dt) {
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty()) {
		// Moved long enough in current direction: change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance) {
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);
		setVelocity(vx, vy);
		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

float Aircraft::getMaxSpeed() const {
	return Table[mType].speed;
}