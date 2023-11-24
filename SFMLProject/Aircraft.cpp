#include "Aircraft.hpp"
#include <iostream>

namespace {
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts) :
	Entity(Table[type].hitpoints),
	mType(type),
	mSprite(textures.get(Table[type].texture)), // convert Aircraft::Type e.g. 'Eagle' enum to Textures::ID enum 
	mFireCommand(),
	mMissileCommand(),
	mFireCountdown(sf::Time::Zero),
	mIsFiring(false),
	mIsLaunchingMissile(false),
	mIsMarkedForRemoval(false),
	mFireRateLevel(0),
	mSpreadLevel(1),
	mMissileAmmo(2),
	mDropPickupCommand(),
	mTravelledDistance(0.f),
	mDirectionIndex(0),
	mHealthDisplay(nullptr),
	mMissileDisplay(nullptr)
{	
	centerOrigin(mSprite);

	mFireCommand.category = Category::PlayerAircraft | Category::EnemyAircraft;
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::PlayerAircraft;
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));
	if (getCategory() == Category::PlayerAircraft) {
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands) {
	if (isDestroyed()) {
		//checkPickupDrop(commands);
		mIsMarkedForRemoval = true;
		return;
	}
	//std::cout << mIsFiring << std::endl;
	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
	updateTexts();
}

unsigned int Aircraft::getCategory() const {
	if (isAllied())
		return Category::PlayerAircraft;
	else
		return Category::EnemyAircraft;
}

sf::FloatRect Aircraft::getBoundingRect() const {
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const {
	return mIsMarkedForRemoval;
}

bool Aircraft::isAllied() const {
	return mType == Eagle;
}

float Aircraft::getMaxSpeed() const {
	return Table[mType].speed;
}

void Aircraft::increaseFireRate() {
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Aircraft::increaseSpread() {
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Aircraft::collectMissiles(unsigned int count) {
	mMissileAmmo += count;
}

void Aircraft::fire() {
	//if (Table[mType].fireInterval != sf::Time::Zero)
	mIsFiring = true;
}

void Aircraft::launchMissile() {
	if (mMissileAmmo > 0) {
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
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

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands) {
	if (!isAllied())
		std::cout << mIsFiring << std::endl;
	if (mIsFiring && mFireCountdown <= sf::Time::Zero) {
		commands.push(mFireCommand);
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero) {
		mFireCountdown -= dt;
	}
	if (mIsLaunchingMissile) {
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const {
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;
	switch (mSpreadLevel) {
	case 1:
		createProjectile(node, type, 0.f, 0.5f, textures);
		break;
	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, 0.33f, 0.33f, textures);
		break;
	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.f, 0.5f, textures);
		createProjectile(node, type, 0.5f, 0.33f, textures);
		break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());
	float sign = isAllied() ? -1.f : 1.f;
	projectile->setPosition(offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::updateTexts() {
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation()); // set health text not rotating when plane rotates
	if (mMissileDisplay) {
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}
